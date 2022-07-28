/*
  Copyright (c) 2015, 2022, Oracle and/or its affiliates.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License, version 2.0,
  as published by the Free Software Foundation.

  This program is also distributed with certain software (including
  but not limited to OpenSSL) that is licensed under separate terms,
  as designated in a particular file or component or in included license
  documentation.  The authors of MySQL hereby grant you an additional
  permission to link the program and your derivative works with the
  separately licensed software that they have included with MySQL.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License, version 2.0, for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "abstract_connection_provider.h"
#include "instance_callback.h"

using namespace Mysql::Tools::Dump;

int64 Abstract_connection_provider::Message_handler_wrapper::pass_message(
  const Mysql::Tools::Base::Message_data& message)
{
  if (m_message_handler != NULL)
  {
    return (*m_message_handler)(message) ? -1 : 0;
  }
  return 0;
}

Abstract_connection_provider::Message_handler_wrapper::Message_handler_wrapper(
  Mysql::I_callable<bool, const Mysql::Tools::Base::Message_data&>*
    message_handler)
  : m_message_handler(message_handler)
{}

Mysql::Tools::Base::Mysql_query_runner*
  Abstract_connection_provider::create_new_runner(
    Mysql::I_callable<bool, const Mysql::Tools::Base::Message_data&>*
      message_handler)
{
  MYSQL* connection= m_connection_factory->create_connection();
  Message_handler_wrapper* message_wrapper=
    new Message_handler_wrapper(message_handler);
  I_callable<int64, const Mysql::Tools::Base::Message_data&>* callback
    = new Mysql::Instance_callback_own<
    int64, const Mysql::Tools::Base::Message_data&,
    Message_handler_wrapper>(
      message_wrapper, &Message_handler_wrapper::pass_message);

  return &((new Mysql::Tools::Base::Mysql_query_runner(connection))
    ->add_message_callback(callback));
}

Abstract_connection_provider::Abstract_connection_provider(
  Mysql::Tools::Base::I_connection_factory* connection_factory)
  : m_connection_factory(connection_factory)
{}
