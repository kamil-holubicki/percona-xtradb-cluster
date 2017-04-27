/* Copyright (c) 2000, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/**
  @file mysys/my_thread.cc
*/

#include "my_thread.h"

#ifdef _WIN32
#include "my_sys.h"        /* my_osmaperr */
#include <errno.h>
#include <process.h>
#include <signal.h>

struct thread_start_parameter
{
  my_start_routine func;
  void *arg;
};


static unsigned int __stdcall win_thread_start(void *p)
{
  struct thread_start_parameter *par= (struct thread_start_parameter *)p;
  my_start_routine func= par->func;
  void *arg= par->arg;
  free(p);
  (*func)(arg);
  return 0;
}
#endif


int my_thread_create(my_thread_handle *thread, const my_thread_attr_t *attr,
                     my_start_routine func, void *arg)
{
#ifndef _WIN32
  return pthread_create(&thread->thread, attr, func, arg);
#else
  struct thread_start_parameter *par;
  unsigned int  stack_size;

  par= (struct thread_start_parameter *)malloc(sizeof(*par));
  if (!par)
    goto error_return;

  par->func= func;
  par->arg= arg;
  stack_size= attr ? attr->dwStackSize : 0;

  thread->handle= (HANDLE)_beginthreadex(NULL, stack_size, win_thread_start,
                                         par, 0, (unsigned int*)&thread->thread);

  if (thread->handle)
  {
    /* Note that JOINABLE is default, so attr == NULL => JOINABLE. */
    if (attr && attr->detachstate == MY_THREAD_CREATE_DETACHED)
    {
      /*
        Close handles for detached threads right away to avoid leaking
        handles. For joinable threads we need the handle during
        my_thread_join. It will be closed there.
      */
      CloseHandle(thread->handle);
      thread->handle= NULL;
    }
    return 0;
  }

  my_osmaperr(GetLastError());
  free(par);

error_return:
  thread->thread= 0;
  thread->handle= NULL;
  return 1;
#endif
}


int my_thread_join(my_thread_handle *thread, void **value_ptr)
{
#ifndef _WIN32
  return pthread_join(thread->thread, value_ptr);
#else
  DWORD  ret;
  int  result= 0;
  ret= WaitForSingleObject(thread->handle, INFINITE);
  if (ret != WAIT_OBJECT_0)
  {
    my_osmaperr(GetLastError());
    result= 1;
  }
  if (thread->handle)
    CloseHandle(thread->handle);
  thread->thread= 0;
  thread->handle= NULL;
  return result;
#endif
}


int my_thread_cancel(my_thread_handle *thread)
{
#ifndef _WIN32
  return pthread_cancel(thread->thread);
#else
  BOOL ok= FALSE;

  if (thread->handle)
  {
     ok= TerminateThread(thread->handle, 0);
     CloseHandle(thread->handle);
  }
  if (ok)
    return 0;

  errno= EINVAL;
  return -1;
#endif
}


void my_thread_exit(void *value_ptr)
{
#ifndef _WIN32
  pthread_exit(value_ptr);
#else
  _endthreadex(0);
#endif
}
