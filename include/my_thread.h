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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA */

/**
  @file include/my_thread.h
  Defines to make different thread packages compatible.
*/

#ifndef MY_THREAD_INCLUDED
#define MY_THREAD_INCLUDED

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#include <mysql/components/services/my_thread_bits.h>

#include "my_compiler.h"
#include "my_config.h"
#include "my_inttypes.h"
#include "my_macros.h"

#ifndef ETIME
#define ETIME ETIMEDOUT             /* For FreeBSD */
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT 145		    /* Win32 doesn't have this */
#endif

/*
  MySQL can survive with 32K, but some glibc libraries require > 128K stack
  To resolve hostnames. Also recursive stored procedures needs stack.
*/
#if defined(__sparc) && (defined(__SUNPRO_CC) || defined(__SUNPRO_C))
#define STACK_MULTIPLIER 2UL
#elif defined HAVE_UBSAN && SIZEOF_CHARP == 4
#define STACK_MULTIPLIER 3UL
#else
#define STACK_MULTIPLIER 1UL
#endif

#if SIZEOF_CHARP > 4
#define DEFAULT_THREAD_STACK	(STACK_MULTIPLIER * 280UL * 1024UL)
#else
#define DEFAULT_THREAD_STACK	(STACK_MULTIPLIER * 216UL * 1024UL)
#endif

#ifdef  __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

static inline int is_timeout(int e) {
#if ETIMEDOUT == ETIME
  return e == ETIMEDOUT;
#else
  return e == ETIMEDOUT || e == ETIME;
#endif
}

C_MODE_START

#ifdef _WIN32
#define MY_THREAD_CREATE_JOINABLE 0
#define MY_THREAD_CREATE_DETACHED 1
typedef void * (__cdecl *my_start_routine)(void *);
#else
#define MY_THREAD_CREATE_JOINABLE PTHREAD_CREATE_JOINABLE
#define MY_THREAD_CREATE_DETACHED PTHREAD_CREATE_DETACHED
typedef void *(* my_start_routine)(void *);
#endif

struct my_thread_handle
{
  my_thread_t thread;
#ifdef _WIN32
  HANDLE handle;
#endif
};

static inline my_thread_t my_thread_self()
{
#ifdef _WIN32
  return GetCurrentThreadId();
#else
  return pthread_self();
#endif
}

static inline int my_thread_equal(my_thread_t t1, my_thread_t t2)
{
#ifdef _WIN32
  return t1 == t2;
#else
  return pthread_equal(t1, t2);
#endif
}

static inline int my_thread_attr_init(my_thread_attr_t *attr)
{
#ifdef _WIN32
  attr->dwStackSize= 0;
  /* Set to joinable by default to match Linux */
  attr->detachstate= MY_THREAD_CREATE_JOINABLE;
  return 0;
#else
  return pthread_attr_init(attr);
#endif
}

static inline int my_thread_attr_destroy(my_thread_attr_t *attr)
{
#ifdef _WIN32
  attr->dwStackSize= 0;
  /* Set to joinable by default to match Linux */
  attr->detachstate= MY_THREAD_CREATE_JOINABLE;
  return 0;
#else
  return pthread_attr_destroy(attr);
#endif
}

static inline int my_thread_attr_setstacksize(my_thread_attr_t *attr,
                                              size_t stacksize)
{
#ifdef _WIN32
  attr->dwStackSize= (DWORD)stacksize;
  return 0;
#else
  return pthread_attr_setstacksize(attr, stacksize);
#endif
}

static inline int my_thread_attr_setdetachstate(my_thread_attr_t *attr,
                                                int detachstate)
{
#ifdef _WIN32
  attr->detachstate= detachstate;
  return 0;
#else
  return pthread_attr_setdetachstate(attr, detachstate);
#endif
}

static inline int my_thread_attr_getstacksize(my_thread_attr_t *attr,
                                              size_t *stacksize)
{
#ifdef _WIN32
  *stacksize= (size_t)attr->dwStackSize;
  return 0;
#else
  return pthread_attr_getstacksize(attr, stacksize);
#endif
}

static inline void my_thread_yield()
{
#ifdef _WIN32
  SwitchToThread();
#else
  sched_yield();
#endif
}

int my_thread_create(my_thread_handle *thread, const my_thread_attr_t *attr,
                     my_start_routine func, void *arg);
int my_thread_join(my_thread_handle *thread, void **value_ptr);
int my_thread_cancel(my_thread_handle *thread);
void my_thread_exit(void *value_ptr) MY_ATTRIBUTE((noreturn));


extern bool my_thread_global_init();
extern void my_thread_global_reinit();
extern void my_thread_global_end();
extern bool my_thread_init();
extern void my_thread_end();

C_MODE_END

#endif /* MY_THREAD_INCLUDED */
