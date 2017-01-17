/*
      Copyright (c) 2013, 2017, Oracle and/or its affiliates. All rights
   reserved.

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

/**
  @file storage/perfschema/table_replication_applier_status_by_coordinator.cc
  Table replication_applier_status_by_coordinator (implementation).
*/

#include "my_dbug.h"
#include "my_global.h"

#ifndef EMBEDDED_LIBRARY
#define HAVE_REPLICATION
#endif /* EMBEDDED_LIBRARY */

#include "pfs_instr.h"
#include "pfs_instr_class.h"
#include "rpl_info.h"
#include "rpl_mi.h"
#include "rpl_msr.h" /* Multisource replication */
#include "rpl_rli.h"
#include "rpl_slave.h"
#include "sql_parse.h"
#include "table_helper.h"
#include "table_replication_applier_status_by_coordinator.h"

THR_LOCK table_replication_applier_status_by_coordinator::m_table_lock;

/* clang-format off */
static const TABLE_FIELD_TYPE field_types[]=
{
  {
    {C_STRING_WITH_LEN("CHANNEL_NAME")},
    {C_STRING_WITH_LEN("char(64)")},
    {NULL, 0}
  },
  {
    {C_STRING_WITH_LEN("THREAD_ID")},
    {C_STRING_WITH_LEN("bigint")},
    {NULL, 0}
  },
  {
    {C_STRING_WITH_LEN("SERVICE_STATE")},
    {C_STRING_WITH_LEN("enum('ON','OFF')")},
    {NULL, 0}
  },
  {
    {C_STRING_WITH_LEN("LAST_ERROR_NUMBER")},
    {C_STRING_WITH_LEN("int(11)")},
    {NULL, 0}
  },
  {
    {C_STRING_WITH_LEN("LAST_ERROR_MESSAGE")},
    {C_STRING_WITH_LEN("varchar(1024)")},
    {NULL, 0}
  },
  {
    {C_STRING_WITH_LEN("LAST_ERROR_TIMESTAMP")},
    { C_STRING_WITH_LEN("timestamp") },
    { NULL, 0}
  },
};
/* clang-format on */

TABLE_FIELD_DEF
table_replication_applier_status_by_coordinator::m_field_def = {6, field_types};

PFS_engine_table_share
  table_replication_applier_status_by_coordinator::m_share = {
    {C_STRING_WITH_LEN("replication_applier_status_by_coordinator")},
    &pfs_readonly_acl,
    table_replication_applier_status_by_coordinator::create,
    NULL, /* write_row */
    NULL, /* delete_all_rows */
    table_replication_applier_status_by_coordinator::get_row_count,
    sizeof(PFS_simple_index), /* ref length */
    &m_table_lock,
    &m_field_def,
    false, /* checked */
    false  /* perpetual */
};

#ifdef HAVE_REPLICATION
bool
PFS_index_rpl_applier_status_by_coord_by_channel::match(Master_info *mi)
{
  if (m_fields >= 1)
  {
    st_row_coordinator row;

    /* Mutex locks not necessary for channel name. */
    row.channel_name_length =
      mi->get_channel() ? (uint)strlen(mi->get_channel()) : 0;
    memcpy(row.channel_name, mi->get_channel(), row.channel_name_length);

    if (!m_key.match(row.channel_name, row.channel_name_length))
    {
      return false;
    }
  }

  return true;
}

bool
PFS_index_rpl_applier_status_by_coord_by_thread::match(Master_info *mi)
{
  if (m_fields >= 1)
  {
    st_row_coordinator row;
    row.thread_id_is_null = true;

    mysql_mutex_lock(&mi->rli->data_lock);

    if (mi->rli->slave_running)
    {
      PSI_thread *psi = thd_get_psi(mi->rli->info_thd);
      PFS_thread *pfs = reinterpret_cast<PFS_thread *>(psi);
      if (pfs)
      {
        row.thread_id = pfs->m_thread_internal_id;
        row.thread_id_is_null = false;
      }
    }

    mysql_mutex_unlock(&mi->rli->data_lock);

    if (row.thread_id_is_null)
    {
      return false;
    }

    if (!m_key.match(row.thread_id))
    {
      return false;
    }
  }

  return true;
}
#endif

PFS_engine_table *
table_replication_applier_status_by_coordinator::create(void)
{
  return new table_replication_applier_status_by_coordinator();
}

table_replication_applier_status_by_coordinator::
  table_replication_applier_status_by_coordinator()
  : PFS_engine_table(&m_share, &m_pos), m_pos(0), m_next_pos(0)
{
}

table_replication_applier_status_by_coordinator::
  ~table_replication_applier_status_by_coordinator()
{
}

void
table_replication_applier_status_by_coordinator::reset_position(void)
{
  m_pos.m_index = 0;
  m_next_pos.m_index = 0;
}

ha_rows
table_replication_applier_status_by_coordinator::get_row_count()
{
#ifdef HAVE_REPLICATION
  return channel_map.get_max_channels();
#else
  return 0;
#endif /* HAVE_REPLICATION */
}

int
table_replication_applier_status_by_coordinator::rnd_next(void)
{
  int res = HA_ERR_END_OF_FILE;

#ifdef HAVE_REPLICATION
  Master_info *mi;

  channel_map.rdlock();

  for (m_pos.set_at(&m_next_pos);
       m_pos.m_index < channel_map.get_max_channels() && res != 0;
       m_pos.next())
  {
    mi = channel_map.get_mi_at_pos(m_pos.m_index);

    /*
      Construct and display SQL Thread's (Coordinator) information in
      'replication_applier_status_by_coordinator' table only in the case of
      multi threaded slave mode. Code should do nothing in the case of single
      threaded slave mode. In case of single threaded slave mode SQL Thread's
      status will be reported as part of
      'replication_applier_status_by_worker' table.
    */
    if (mi && mi->host[0] && mi->rli && mi->rli->get_worker_count() > 0)
    {
      res = make_row(mi);
      m_next_pos.set_after(&m_pos);
    }
  }

  channel_map.unlock();
#endif /* HAVE_REPLICATION */

  return res;
}

int
table_replication_applier_status_by_coordinator::rnd_pos(
  const void *pos MY_ATTRIBUTE((unused)))
{
  int res = HA_ERR_RECORD_DELETED;

#ifdef HAVE_REPLICATION
  Master_info *mi = NULL;

  set_position(pos);

  channel_map.rdlock();

  if ((mi = channel_map.get_mi_at_pos(m_pos.m_index)))
  {
    res = make_row(mi);
  }

  channel_map.unlock();
#endif /* HAVE_REPLICATION */

  return res;
}

int
table_replication_applier_status_by_coordinator::index_init(
  uint idx MY_ATTRIBUTE((unused)), bool)
{
#ifdef HAVE_REPLICATION
  PFS_index_rpl_applier_status_by_coord *result = NULL;

  switch (idx)
  {
  case 0:
    result = PFS_NEW(PFS_index_rpl_applier_status_by_coord_by_channel);
    break;
  case 1:
    result = PFS_NEW(PFS_index_rpl_applier_status_by_coord_by_thread);
    break;
  default:
    DBUG_ASSERT(false);
    break;
  }
  m_opened_index = result;
  m_index = result;
#endif
  return 0;
}

int
table_replication_applier_status_by_coordinator::index_next(void)
{
  int res = HA_ERR_END_OF_FILE;

#ifdef HAVE_REPLICATION
  Master_info *mi;

  channel_map.rdlock();

  for (m_pos.set_at(&m_next_pos);
       m_pos.m_index < channel_map.get_max_channels() && res != 0;
       m_pos.next())
  {
    mi = channel_map.get_mi_at_pos(m_pos.m_index);

    /*
      Construct and display SQL Thread's (Coordinator) information in
      'replication_applier_status_by_coordinator' table only in the case of
      multi threaded slave mode. Code should do nothing in the case of single
      threaded slave mode. In case of single threaded slave mode SQL Thread's
      status will be reported as part of
      'replication_applier_status_by_worker' table.
    */
    if (mi && mi->host[0] && mi->rli && mi->rli->get_worker_count() > 0)
    {
      if (m_opened_index->match(mi))
      {
        res = make_row(mi);
        m_next_pos.set_after(&m_pos);
      }
    }
  }

  channel_map.unlock();
#endif /* HAVE_REPLICATION */

  return res;
}

#ifdef HAVE_REPLICATION
int
table_replication_applier_status_by_coordinator::make_row(Master_info *mi)
{
  DBUG_ASSERT(mi != NULL);
  DBUG_ASSERT(mi->rli != NULL);

  mysql_mutex_lock(&mi->rli->data_lock);

  m_row.channel_name_length = strlen(mi->get_channel());
  memcpy(
    m_row.channel_name, (char *)mi->get_channel(), m_row.channel_name_length);

  if (mi->rli->slave_running)
  {
    PSI_thread *psi = thd_get_psi(mi->rli->info_thd);
    PFS_thread *pfs = reinterpret_cast<PFS_thread *>(psi);
    if (pfs)
    {
      m_row.thread_id = pfs->m_thread_internal_id;
      m_row.thread_id_is_null = false;
    }
    else
    {
      m_row.thread_id_is_null = true;
    }
  }
  else
  {
    m_row.thread_id_is_null = true;
  }

  if (mi->rli->slave_running)
  {
    m_row.service_state = PS_RPL_YES;
  }
  else
  {
    m_row.service_state = PS_RPL_NO;
  }

  mysql_mutex_lock(&mi->rli->err_lock);

  m_row.last_error_number = (long int)mi->rli->last_error().number;
  m_row.last_error_message_length = 0;
  m_row.last_error_timestamp = 0;

  /** if error, set error message and timestamp */
  if (m_row.last_error_number)
  {
    char *temp_store = (char *)mi->rli->last_error().message;
    m_row.last_error_message_length = strlen(temp_store);
    memcpy(
      m_row.last_error_message, temp_store, m_row.last_error_message_length);

    /** time in millisecond since epoch */
    m_row.last_error_timestamp = (ulonglong)mi->rli->last_error().skr * 1000000;
  }

  mysql_mutex_unlock(&mi->rli->err_lock);
  mysql_mutex_unlock(&mi->rli->data_lock);

  return 0;
}
#endif /* HAVE_REPLICATION */

int
table_replication_applier_status_by_coordinator::read_row_values(
  TABLE *table MY_ATTRIBUTE((unused)),
  unsigned char *buf MY_ATTRIBUTE((unused)),
  Field **fields MY_ATTRIBUTE((unused)),
  bool read_all MY_ATTRIBUTE((unused)))
{
#ifdef HAVE_REPLICATION
  Field *f;

  DBUG_ASSERT(table->s->null_bytes == 1);
  buf[0] = 0;

  for (; (f = *fields); fields++)
  {
    if (read_all || bitmap_is_set(table->read_set, f->field_index))
    {
      switch (f->field_index)
      {
      case 0: /* channel_name */
        set_field_char_utf8(f, m_row.channel_name, m_row.channel_name_length);
        break;
      case 1: /*thread_id*/
        if (!m_row.thread_id_is_null)
        {
          set_field_ulonglong(f, m_row.thread_id);
        }
        else
        {
          f->set_null();
        }
        break;
      case 2: /*service_state*/
        set_field_enum(f, m_row.service_state);
        break;
      case 3: /*last_error_number*/
        set_field_ulong(f, m_row.last_error_number);
        break;
      case 4: /*last_error_message*/
        set_field_varchar_utf8(
          f, m_row.last_error_message, m_row.last_error_message_length);
        break;
      case 5: /*last_error_timestamp*/
        set_field_timestamp(f, m_row.last_error_timestamp);
        break;
      default:
        DBUG_ASSERT(false);
      }
    }
  }
  return 0;
#else
  return HA_ERR_RECORD_DELETED;
#endif /* HAVE_REPLICATION */
}
