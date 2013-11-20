/* Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved.

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
  @file

  @brief Contains wrapper functions for memory allocation and deallocation.
  This includes generic functions to be called from the binlogapi library,
  which call the appropriate corresponding function, depending on whether
  the library is compiled independently, or with the MySQL server.
*/

#ifndef WRAPPER_FUNCTIONS_H
#define WRAPPER_FUNCTIONS_H

#include "config.h"

#if HAVE_MYSYS
#include "my_sys.h"
extern PSI_memory_key key_memory_Incident_log_event_message;
extern PSI_memory_key key_memory_Rows_query_log_event_rows_query;
extern PSI_memory_key key_memory_log_event;
#else
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <cstdlib>
#include <cstring>
#endif

/**
  This is a wrapper function, and returns a pointer to a new string which is
  a duplicate of the input string. The terminating Null character is added.

  If compiled with MySQL server,the strndup function from the mysys library is
  called, which allow instrumenting memory allocated. Else, the standard
  string function is called.

  @param destination The string to be duplicated
  @param n The number of bytes to be copied

  @return The duplicated string, or NULL if insufficient memory was available.
*/
inline const char* bapi_strndup(const char *destination, size_t n)
{
#if HAVE_MYSYS
/* Call the function in mysys library, required for memory instrumentation */
  return my_strndup(key_memory_log_event, destination, n, MYF(MY_WME));
#else
  return strndup(destination, n);
#endif
}

/**
  This is a wrapper function, and returns a pointer to a new memory with the
  contents copied from the input memory pointer, upto a given length

  @param source Pointer to the buffer from which data is to be copied
  @param len Length upto which the source should be copied

  @return dest pointer to a new memory if allocation was successful
          NULL otherwise
*/
inline const char* bapi_memdup(const char* source, size_t len)
{
  const char* dest;
#if HAVE_MYSYS
  /* Call the function in mysys library, required for memory instrumentation */
  dest= (const char*)my_memdup(key_memory_log_event, source, len, MYF(MY_WME));
#else
  dest= (const char*)malloc(len);
  if (dest)
    memcpy(&event_buf, &source, len);
#endif
  return dest;
}


/**
  This is a wrapper function inorder to free the memory allocated from the heap
  in the binlogapi library.

  If compiled with the MySQL server, and memory is allocated using memory
  allocating methods from the mysys library, my_free is called. Otherwise,
  the standard free() is called from the function.

  @param Pointer to the memory which is to be freed.
*/
inline void bapi_free(void* ptr)
{
#if HAVE_MYSYS
  return my_free(ptr);
#else
  return free(ptr);
#endif
}

#endif


