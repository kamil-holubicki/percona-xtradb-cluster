/*
  Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <boost/date_time.hpp>
#include <stddef.h>
#include <functional>

#include "object_queue.h"
#include "this_thread.h"

using namespace Mysql::Tools::Dump;
using std::placeholders::_1;

void Object_queue::add_ready_items_to_queue(
  std::map<const I_dump_task*,
  std::vector<Item_processing_data*>* > ::iterator it)
{
  for (std::vector<Item_processing_data*>::iterator item_iterator=
    it->second->begin();
    item_iterator != it->second->end();
  ++item_iterator)
  {
    m_items_ready_for_processing.push(*item_iterator);
  }
  delete it->second;
  m_tasks_map.erase(it);
}

void Object_queue::task_availability_callback(
  const Abstract_dump_task* available_task)
{
  my_boost::mutex::scoped_lock lock(m_queue_mutex);

  std::map<const I_dump_task*, std::vector<Item_processing_data*>* >
    ::iterator it= m_tasks_map.find(available_task);
  if (it != m_tasks_map.end())
  {
    this->add_ready_items_to_queue(it);
  }
}

void Object_queue::queue_thread()
{
  (*this->m_thread_callback)(true);
  while (true)
  {
    /* check if any errors are generated by main or child threads */
    if(m_program->get_error_code())
      stop_queue();

    if (m_is_queue_running.load() == false)
      break;

    Item_processing_data* item_to_process= NULL;
    {
      my_boost::mutex::scoped_lock lock(m_queue_mutex);
      if (m_items_ready_for_processing.size() > 0)
      {
        item_to_process= m_items_ready_for_processing.front();
        m_items_ready_for_processing.pop();
      }
    }

    if (item_to_process != NULL)
    {
      this->format_object(item_to_process);
      this->object_processing_ends(item_to_process);
    }

    /**
      In case there are no items to be processed from the queue then
      in such a scenario the current thread will be infinitely hung in
      this loop without allowing context switch to happen, so that other
      executing threads can further progress. Thus we need this thread
      to sleep so that other threads can proceed to complete its execution
      or allow main thread to insert new items into the queue.
    */
    {
      my_boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }

  }
  (*this->m_thread_callback)(false);
}

void Object_queue::read_object(Item_processing_data* item_to_process)
{
  this->object_processing_starts(item_to_process);

  Abstract_dump_task* dump_task= dynamic_cast<Abstract_dump_task*>(
    item_to_process->get_process_task_object());

  if (dump_task == NULL)
  {
    (*this->get_message_handler())(
      Mysql::Tools::Base::Message_data(
      0, "Not supported operation called.",
      Mysql::Tools::Base::Message_type_error));
  }

  my_boost::mutex::scoped_lock lock(m_queue_mutex);
  /*
    Check if all dependencies are already met, if so, we can directly add
    this processing item to queue. If no, we will create completion callback
    to handle addition to queue when ready, but this may be called in the
    meantime, so we need to check this statement again at the end.
    */
  if (dump_task->can_be_executed())
  {
    m_items_ready_for_processing.push(item_to_process);
    return;
  }

  std::map<const I_dump_task*, std::vector<Item_processing_data*>* >
    ::iterator it= m_tasks_map.find(dump_task);

  std::vector<Item_processing_data*>* list;

  if (it == m_tasks_map.end())
  {
    list= new std::vector<Item_processing_data*>();
    m_tasks_map.insert(make_pair(dump_task,
      list));
    dump_task->register_execution_availability_callback(
      &m_task_availability_callback);
  }
  else
    list= it->second;

  list->push_back(item_to_process);

  /*
    Check if executing threads haven't completed this task dependencies in
    meantime, if so, we must revert what we have done with waiting task list.
    */
  if (dump_task->can_be_executed())
    this->add_ready_items_to_queue(m_tasks_map.find(dump_task));
}

void Object_queue::stop_queue()
{
  m_is_queue_running= false;
}

Object_queue::~Object_queue()
{
  m_is_queue_running= false;
  m_thread_group.join_all();
  delete m_thread_callback;
  std::map<const I_dump_task*, std::vector<Item_processing_data*>* >
    ::iterator it= m_tasks_map.begin();
  for(;it != m_tasks_map.end(); ++it)
    delete it->second;
}

Object_queue::Object_queue(
  std::function<bool(const Mysql::Tools::Base::Message_data&)>*
    message_handler, Simple_id_generator* object_id_generator,
  uint threads_count, std::function<void(bool)>* thread_callback,
  Mysql::Tools::Base::Abstract_program* program)
  : Abstract_object_reader_wrapper(message_handler, object_id_generator),
  m_task_availability_callback(
    std::bind(&Object_queue::task_availability_callback, this, _1)),
  m_is_queue_running(true),
  m_thread_callback(thread_callback),
  m_program(program)
{
  for (int thread= threads_count; thread-- > 0;)
  {
    m_thread_group.create_thread(
      std::bind(&Object_queue::queue_thread, this));
  }
}
