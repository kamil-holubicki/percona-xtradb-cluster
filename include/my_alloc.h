/* Copyright (c) 2000, 2010, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   Without limiting anything contained in the foregoing, this file,
   which is part of C Driver for MySQL (Connector/C), is also subject to the
   Universal FOSS Exception, version 1.0, a copy of which can be found at
   http://oss.oracle.com/licenses/universal-foss-exception.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/* 
   Data structures for mysys/my_alloc.c (root memory allocator)
*/

#ifndef _my_alloc_h
#define _my_alloc_h

#define ALLOC_MAX_BLOCK_TO_DROP			4096
#define ALLOC_MAX_BLOCK_USAGE_BEFORE_DROP	10

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_used_mem
{				   /* struct for once_alloc (block) */
  struct st_used_mem *next;	   /* Next block in use */
  unsigned int	left;		   /* memory left in block  */
  unsigned int	size;		   /* size of block */
} USED_MEM;


typedef struct st_mem_root
{
  USED_MEM *free;                  /* blocks with free memory in it */
  USED_MEM *used;                  /* blocks almost without free memory */
  USED_MEM *pre_alloc;             /* preallocated block */
  /* if block have less memory it will be put in 'used' list */
  size_t min_malloc;
  size_t block_size;               /* initial block size */
  unsigned int block_num;          /* allocated blocks counter */
  /* 
     first free block in queue test counter (if it exceed 
     MAX_BLOCK_USAGE_BEFORE_DROP block will be dropped in 'used' list)
  */
  unsigned int first_block_usage;

  void (*error_handler)(void);
} MEM_ROOT;

#ifdef  __cplusplus
}
#endif

#endif
