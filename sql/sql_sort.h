#ifndef SQL_SORT_INCLUDED
#define SQL_SORT_INCLUDED

/* Copyright (c) 2000, 2011, Oracle and/or its affiliates. All rights reserved.

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
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#include "m_string.h"                           /* memset */
#include "my_global.h"                          /* uchar */
#include "my_base.h"                            /* ha_rows */
#include "my_sys.h"                             /* qsort2_cmp */

typedef struct st_buffpek BUFFPEK;
typedef struct st_queue QUEUE;
typedef struct st_sort_field SORT_FIELD;

class Field;
struct TABLE;

/* Defines used by filesort and uniques */

#define MERGEBUFF		7
#define MERGEBUFF2		15

/*
   The structure SORT_ADDON_FIELD describes a fixed layout
   for field values appended to sorted values in records to be sorted
   in the sort buffer.
   Only fixed layout is supported now.
   Null bit maps for the appended values is placed before the values 
   themselves. Offsets are from the last sorted field, that is from the
   record referefence, which is still last component of sorted records.
   It is preserved for backward compatiblility.
   The structure is used tp store values of the additional fields 
   in the sort buffer. It is used also when these values are read
   from a temporary file/buffer. As the reading procedures are beyond the
   scope of the 'filesort' code the values have to be retrieved via
   the callback function 'unpack_addon_fields'.
*/

typedef struct st_sort_addon_field {  /* Sort addon packed field */
  Field *field;          /* Original field */
  uint   offset;         /* Offset from the last sorted field */
  uint   null_offset;    /* Offset to to null bit from the last sorted field */
  uint   length;         /* Length in the sort buffer */
  uint8  null_bit;       /* Null bit mask for the field */
} SORT_ADDON_FIELD;

typedef struct st_buffpek {		/* Struktur om sorteringsbuffrarna */
  my_off_t file_pos;			/* Where we are in the sort file */
  uchar *base,*key;			/* key pointers */
  ha_rows count;			/* Number of rows in table */
  ulong mem_count;			/* numbers of keys in memory */
  ulong max_keys;			/* Max keys in buffert */
} BUFFPEK;

struct BUFFPEK_COMPARE_CONTEXT
{
  qsort_cmp2 key_compare;
  const void *key_compare_arg;
};

class Sort_param {
public:
  uint rec_length;            // Length of sorted records.
  uint sort_length;           // Length of sorted columns.
  uint ref_length;            // Length of record ref.
  uint addon_length;          // Length of added packed fields.
  uint res_length;            // Length of records in final sorted file/buffer.
  uint max_keys_per_buffer;   // Max keys / buffer.
  ha_rows max_rows;           // Select limit, or HA_POS_ERROR if unlimited.
  ha_rows examined_rows;      // Number of examined rows.
  TABLE *sort_form;           // For quicker make_sortkey.
  SORT_FIELD *local_sortorder;
  SORT_FIELD *end;
  SORT_ADDON_FIELD *addon_field; // Descriptors for companion fields.
  uchar *unique_buff;
  bool not_killable;
  char* tmp_buffer;
  // The fields below are used only by Unique class.
  qsort2_cmp compare;
  BUFFPEK_COMPARE_CONTEXT cmp_context;

  Sort_param()
  {
    memset(this, 0, sizeof(*this));
  }
  void init_for_filesort(uint sortlen, TABLE *table,
                         ulong max_length_for_sort_data,
                         ha_rows maxrows, bool sort_positions);
};


int merge_many_buff(Sort_param *param, uchar *sort_buffer,
		    BUFFPEK *buffpek,
		    uint *maxbuffer, IO_CACHE *t_file);
uint read_to_buffer(IO_CACHE *fromfile,BUFFPEK *buffpek,
		    uint sort_length);
int merge_buffers(Sort_param *param,IO_CACHE *from_file,
                  IO_CACHE *to_file, uchar *sort_buffer,
                  BUFFPEK *lastbuff,BUFFPEK *Fb,
                  BUFFPEK *Tb,int flag);
void reuse_freed_buff(QUEUE *queue, BUFFPEK *reuse, uint key_length);

#endif /* SQL_SORT_INCLUDED */
