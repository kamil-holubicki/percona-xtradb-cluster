/*
   Portions Copyright (c) 2015-Present, Facebook, Inc.
   Portions Copyright (c) 2012, Monty Program Ab

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifdef USE_PRAGMA_IMPLEMENTATION
#pragma implementation  // gcc: Class implementation
#endif

/* The C++ file's header */
#include "./rdb_threads.h"

/* MySQL header files */
#include "sql_class.h"

namespace myrocks {

void *Rdb_thread::thread_func(void *const thread_ptr) {
  assert(thread_ptr != nullptr);
  my_thread_init();
  Rdb_thread *const thread = static_cast<Rdb_thread *>(thread_ptr);

  if (!thread->m_run_once.exchange(true)) {
    thread->run();

    thread->uninit();
  }

  my_thread_end();
  return nullptr;
}

void Rdb_thread::init(
#ifdef HAVE_PSI_INTERFACE
    my_core::PSI_mutex_key stop_bg_psi_mutex_key,
    my_core::PSI_cond_key stop_bg_psi_cond_key
#endif
) {
  assert(!m_run_once);
  mysql_mutex_init(stop_bg_psi_mutex_key, &m_signal_mutex, MY_MUTEX_INIT_FAST);
  mysql_cond_init(stop_bg_psi_cond_key, &m_signal_cond);
}

void Rdb_thread::uninit() {
  mysql_mutex_destroy(&m_signal_mutex);
  mysql_cond_destroy(&m_signal_cond);
}

int Rdb_thread::create_thread(const std::string &thread_name
#ifdef HAVE_PSI_INTERFACE
                              ,
                              PSI_thread_key background_psi_thread_key
#endif
) {
  assert(!thread_name.empty());

  int err = mysql_thread_create(background_psi_thread_key, &m_handle, nullptr,
                                thread_func, this);

  return err;
}

void Rdb_thread::signal(const bool stop_thread) {
  RDB_MUTEX_LOCK_CHECK(m_signal_mutex);

  if (stop_thread) {
    m_killed = THD::KILL_CONNECTION;
  }

  mysql_cond_signal(&m_signal_cond);

  RDB_MUTEX_UNLOCK_CHECK(m_signal_mutex);
}

}  // namespace myrocks
