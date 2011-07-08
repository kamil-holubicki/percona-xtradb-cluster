/* Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#ifndef PFS_INSTR_H
#define PFS_INSTR_H

/**
  @file storage/perfschema/pfs_instr.h
  Performance schema instruments (declarations).
*/

struct PFS_mutex_class;
struct PFS_rwlock_class;
struct PFS_cond_class;
struct PFS_file_class;
struct PFS_table_share;
struct PFS_thread_class;
struct PFS_socket_class;

#ifdef __WIN__
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include "pfs_lock.h"
#include "pfs_stat.h"
#include "pfs_instr_class.h"
#include "pfs_events_waits.h"
#include "pfs_events_stages.h"
#include "pfs_events_statements.h"
#include "pfs_server.h"
#include "lf.h"
#include "pfs_con_slice.h"

/**
  @addtogroup Performance_schema_buffers
  @{
*/

struct PFS_thread;

/** Base structure for wait instruments. */
struct PFS_instr
{
  /** Internal lock. */
  pfs_lock m_lock;
  /** Enabled flag. */
  bool m_enabled;
  /** Timed flag. */
  bool m_timed;
  /** Instrument wait statistics. */
  PFS_single_stat m_wait_stat;
};

/** Instrumented mutex implementation. @see PSI_mutex. */
struct PFS_mutex : public PFS_instr
{
  /** Mutex identity, typically a pthread_mutex_t. */
  const void *m_identity;
  /** Mutex class. */
  PFS_mutex_class *m_class;
  /** Instrument wait statistics. */
  PFS_single_stat m_wait_stat;
  /**
    Mutex lock usage statistics.
    This statistic is not exposed in user visible tables yet.
  */
  PFS_single_stat m_lock_stat;
  /** Current owner. */
  PFS_thread *m_owner;
  /**
    Timestamp of the last lock.
    This statistic is not exposed in user visible tables yet.
  */
  ulonglong m_last_locked;
};

/** Instrumented rwlock implementation. @see PSI_rwlock. */
struct PFS_rwlock : public PFS_instr
{
  /** RWLock identity, typically a pthread_rwlock_t. */
  const void *m_identity;
  /** RWLock class. */
  PFS_rwlock_class *m_class;
  /** Instrument wait statistics. */
  PFS_single_stat m_wait_stat;
  /**
    RWLock read lock usage statistics.
    This statistic is not exposed in user visible tables yet.
  */
  PFS_single_stat m_read_lock_stat;
  /**
    RWLock write lock usage statistics.
    This statistic is not exposed in user visible tables yet.
  */
  PFS_single_stat m_write_lock_stat;
  /** Current writer thread. */
  PFS_thread *m_writer;
  /** Current count of readers. */
  uint m_readers;
  /**
    Timestamp of the last write.
    This statistic is not exposed in user visible tables yet.
  */
  ulonglong m_last_written;
  /**
    Timestamp of the last read.
    This statistic is not exposed in user visible tables yet.
  */
  ulonglong m_last_read;
};

/** Instrumented cond implementation. @see PSI_cond. */
struct PFS_cond : public PFS_instr
{
  /** Condition identity, typically a pthread_cond_t. */
  const void *m_identity;
  /** Condition class. */
  PFS_cond_class *m_class;
  /** Instrument wait statistics. */
  PFS_single_stat m_wait_stat;
  /** Condition instance usage statistics. */
  PFS_cond_stat m_cond_stat;
};

/** Instrumented File and FILE implementation. @see PSI_file. */
struct PFS_file : public PFS_instr
{
  uint32 get_version()
  { return m_lock.get_version(); }

  /** File name. */
  char m_filename[FN_REFLEN];
  /** File name length in bytes. */
  uint m_filename_length;
  /** File class. */
  PFS_file_class *m_class;
  /** Instrument wait statistics. */
  PFS_single_stat m_wait_stat;
  /** File usage statistics. */
  PFS_file_stat m_file_stat;
};

/** Instrumented table implementation. @see PSI_table. */
struct PFS_table
{
  /**
    True if table io instrumentation is enabled.
    This flag is computed.
  */
  bool m_io_enabled;
  /**
    True if table lock instrumentation is enabled.
    This flag is computed.
  */
  bool m_lock_enabled;
  /**
    True if table io instrumentation is timed.
    This flag is computed.
  */
  bool m_io_timed;
  /**
    True if table lock instrumentation is timed.
    This flag is computed.
  */
  bool m_lock_timed;

public:
  /**
    Aggregate this table handle statistics to the parents.
    Only use this method for handles owned by the calling code.
    @sa sanitized_aggregate.
  */
  void aggregate(void)
  { return safe_aggregate(& m_table_stat, m_share, m_opening_thread); }

  /**
    Aggregate this table handle statistics to the parents.
    This method is safe to call on handles not owned by the calling code.
    @sa aggregate
    @sa sanitized_aggregate_io
    @sa sanitized_aggregate_lock
  */
  void sanitized_aggregate(void);

  /**
    Aggregate this table handle io statistics to the parents.
    This method is safe to call on handles not owned by the calling code.
  */
  void sanitized_aggregate_io(void);

  /**
    Aggregate this table handle lock statistics to the parents.
    This method is safe to call on handles not owned by the calling code.
  */
  void sanitized_aggregate_lock(void);

  /** Internal lock. */
  pfs_lock m_lock;
  /** Owner. */
  PFS_thread *m_opening_thread;
  /** Table share. */
  PFS_table_share *m_share;
  /** Table identity, typically a handler. */
  const void *m_identity;
  /** Table statistics. */
  PFS_table_stat m_table_stat;

private:
  static void safe_aggregate(PFS_table_stat *stat,
                             PFS_table_share *safe_share,
                             PFS_thread *safe_thread);
  static void safe_aggregate_io(PFS_table_stat *stat,
                                PFS_table_share *safe_share,
                                PFS_thread *safe_thread);
  static void safe_aggregate_lock(PFS_table_stat *stat,
                                  PFS_table_share *safe_share,
                                  PFS_thread *safe_thread);
};

/** Instrumented socket implementation. @see PSI_socket. */
struct PFS_socket : public PFS_instr
{
  uint32 get_version()
  { return m_lock.get_version(); }

  /** Socket identity, typically int */
  const void *m_identity;
  /** Owning thread, if applicable */
  PFS_thread *m_thread_owner;
  /** Socket file descriptor */
  uint m_fd;
  /** Raw socket address */
  struct sockaddr m_sock_addr;
  /** Length of address */
  socklen_t m_addr_len;
  /** Idle flag. */
  bool m_idle;
  /** Socket class. */
  PFS_socket_class *m_class;
  /** Socket usage statistics. */
  PFS_socket_stat m_socket_stat;
};

/**
  @def WAIT_STACK_LOGICAL_SIZE
  Maximum number of nested waits.
*/
#define WAIT_STACK_LOGICAL_SIZE 3
/**
  @def WAIT_STACK_BOTTOM
  Maximum number dummy waits records.
  One dummy record is reserved for the parent stage / statement,
  at the bottom of the wait stack.
*/
#define WAIT_STACK_BOTTOM 1
/**
  @def WAIT_STACK_SIZE
  Physical size of the waits stack
*/
#define WAIT_STACK_SIZE (WAIT_STACK_BOTTOM + WAIT_STACK_LOGICAL_SIZE)

/** Max size of the statements stack. */
extern uint statement_stack_max;

/**
  @def PFS_MAX_ALLOC_RETRY
  Maximum number of times the code attempts to allocate an item
  from internal buffers, before giving up.
*/
#define PFS_MAX_ALLOC_RETRY 1000

/** The maximun number of passes in @sa PFS_scan. */
#define PFS_MAX_SCAN_PASS 2

/**
  Helper to scan circular buffers.
  Given a buffer of size [0, max_size - 1],
  and a random starting point in the buffer,
  this helper returns up to two [first, last -1] intervals that:
  - fit into the [0, max_size - 1] range,
  - have a maximum combined length of at most PFS_MAX_ALLOC_RETRY.
*/
struct PFS_scan
{
public:
  /**
    Initialize a new scan.
    @param random a random index to start from
    @param max_size the max size of the interval to scan
  */
  void init(uint random, uint max_size);

  /**
    Predicate, has a next pass.
    @return true if there is a next pass to perform.
  */
  bool has_pass() const
  { return (m_pass < m_pass_max); }

  /**
    Iterator, proceed to the next pass.
  */
  void next_pass()
  { m_pass++; }
  
  /** First index for this pass. */
  uint first() const
  { return m_first[m_pass]; }

  /** Last index for this pass. */
  uint last() const
  { return m_last[m_pass]; }

private:
  /** Current pass. */
  uint m_pass;
  /** Maximum number of passes. */
  uint m_pass_max;
  /** First element for each pass. */
  uint m_first[PFS_MAX_SCAN_PASS];
  /** Last element for each pass. */
  uint m_last[PFS_MAX_SCAN_PASS];
};


/** Instrumented thread implementation. @see PSI_thread. */
struct PFS_thread : PFS_connection_slice
{
  static PFS_thread* get_current_thread(void);

  /** Thread instrumentation flag. */
  bool m_enabled;
  /** Size of @c m_events_waits_stack. */
  uint m_events_waits_count;
  /** Event ID counter */
  ulonglong m_event_id;
  /** Internal lock. */
  pfs_lock m_lock;
  /** Pins for filename_hash. */
  LF_PINS *m_filename_hash_pins;
  /** Pins for table_share_hash. */
  LF_PINS *m_table_share_hash_pins;
  /** Pins for setup_actor_hash. */
  LF_PINS *m_setup_actor_hash_pins;
  /** Pins for setup_object_hash. */
  LF_PINS *m_setup_object_hash_pins;
  /** Internal thread identifier, unique. */
  ulong m_thread_internal_id;
  /** Parent internal thread identifier. */
  ulong m_parent_thread_internal_id;
  /** External (SHOW PROCESSLIST) thread identifier, not unique. */
  ulong m_thread_id;
  /** Thread class. */
  PFS_thread_class *m_class;
  /**
    Stack of events waits.
    This member holds the data for the table PERFORMANCE_SCHEMA.EVENTS_WAITS_CURRENT.
    Note that stack[0] is a dummy record that represents the parent stage/statement.
    For example, assuming the following tree:
    - STAGE ID 100
      - WAIT ID 101, parent STAGE 100
        - WAIT ID 102, parent wait 101
    the data in the stack will be:
    stack[0].m_event_id= 100, set by the stage instrumentation
    stack[0].m_event_type= STAGE, set by the stage instrumentation
    stack[0].m_nesting_event_id= unused
    stack[0].m_nesting_event_type= unused
    stack[1].m_event_id= 101
    stack[1].m_event_type= WAIT
    stack[1].m_nesting_event_id= stack[0].m_event_id= 100
    stack[1].m_nesting_event_type= stack[0].m_event_type= STAGE
    stack[2].m_event_id= 102
    stack[2].m_event_type= WAIT
    stack[2].m_nesting_event_id= stack[1].m_event_id= 101
    stack[2].m_nesting_event_type= stack[1].m_event_type= WAIT

    The whole point of the stack[0] record is to allow this optimization
    in the code, in the instrumentation for wait events:
      wait->m_nesting_event_id= (wait-1)->m_event_id;
      wait->m_nesting_event_type= (wait-1)->m_event_type;
    This code works for both the top level wait, and nested waits,
    and works without if conditions, which helps performances.
  */
  PFS_events_waits m_events_waits_stack[WAIT_STACK_SIZE];
  /** True if the circular buffer @c m_waits_history is full. */
  bool m_waits_history_full;
  /** Current index in the circular buffer @c m_waits_history. */
  uint m_waits_history_index;
  /**
    Waits history circular buffer.
    This member holds the data for the table
    PERFORMANCE_SCHEMA.EVENTS_WAITS_HISTORY.
  */
  PFS_events_waits *m_waits_history;

  /** True if the circular buffer @c m_stages_history is full. */
  bool m_stages_history_full;
  /** Current index in the circular buffer @c m_stages_history. */
  uint m_stages_history_index;
  /**
    Stages history circular buffer.
    This member holds the data for the table
    PERFORMANCE_SCHEMA.EVENTS_STAGES_HISTORY.
  */
  PFS_events_stages *m_stages_history;

  /** True if the circular buffer @c m_statements_history is full. */
  bool m_statements_history_full;
  /** Current index in the circular buffer @c m_statements_history. */
  uint m_statements_history_index;
  /**
    Statements history circular buffer.
    This member holds the data for the table
    PERFORMANCE_SCHEMA.EVENTS_STATEMENTS_HISTORY.
  */
  PFS_events_statements *m_statements_history;

  /** User name. */
  char m_username[USERNAME_LENGTH];
  /** Length of @c m_username. */
  uint m_username_length;
  /** Host name. */
  char m_hostname[HOSTNAME_LENGTH];
  /** Length of @c m_hostname. */
  uint m_hostname_length;
  /** Database name. */
  char m_dbname[NAME_LEN];
  /** Length of @c m_dbname. */
  uint m_dbname_length;
  /** Current command. */
  int m_command;
  /** Start time. */
  time_t m_start_time;
  /** Processlist state. */
  const char *m_processlist_state_ptr;
  /** Length of @c m_processlist_state_ptr. */
  uint m_processlist_state_length;
  /** Processlist info. */
  const char *m_processlist_info_ptr;
  /** Length of @c m_processlist_info_length. */
  uint m_processlist_info_length;

  PFS_events_stages m_stage_current;

  /** Size of @c m_events_statements_stack. */
  uint m_events_statements_count;
  PFS_events_statements *m_statement_stack;
};

extern PFS_single_stat *global_instr_class_waits_array;
extern PFS_stage_stat *global_instr_class_stages_array;
extern PFS_statement_stat *global_instr_class_statements_array;

PFS_mutex *sanitize_mutex(PFS_mutex *unsafe);
PFS_rwlock *sanitize_rwlock(PFS_rwlock *unsafe);
PFS_cond *sanitize_cond(PFS_cond *unsafe);
PFS_thread *sanitize_thread(PFS_thread *unsafe);
const char *sanitize_file_name(const char *unsafe);
PFS_file *sanitize_file(PFS_file *unsafe);
PFS_socket *sanitize_socket(PFS_socket *unsafe);

int init_instruments(const PFS_global_param *param);
void cleanup_instruments();
int init_file_hash();
void cleanup_file_hash();
PFS_mutex* create_mutex(PFS_mutex_class *mutex_class, const void *identity);
void destroy_mutex(PFS_mutex *pfs);
PFS_rwlock* create_rwlock(PFS_rwlock_class *klass, const void *identity);
void destroy_rwlock(PFS_rwlock *pfs);
PFS_cond* create_cond(PFS_cond_class *klass, const void *identity);
void destroy_cond(PFS_cond *pfs);

PFS_thread* create_thread(PFS_thread_class *klass, const void *identity,
                          ulong thread_id);

void destroy_thread(PFS_thread *pfs);

PFS_file* find_or_create_file(PFS_thread *thread, PFS_file_class *klass,
                              const char *filename, uint len);

void release_file(PFS_file *pfs);
void destroy_file(PFS_thread *thread, PFS_file *pfs);
PFS_table* create_table(PFS_table_share *share, PFS_thread *opening_thread,
                        const void *identity);
void destroy_table(PFS_table *pfs);

PFS_socket* create_socket(PFS_socket_class *socket_class, const void *identity);
void release_socket(PFS_socket *pfs);
void destroy_socket(PFS_socket *pfs);

/* For iterators and show status. */

extern ulong mutex_max;
extern ulong mutex_lost;
extern ulong rwlock_max;
extern ulong rwlock_lost;
extern ulong cond_max;
extern ulong cond_lost;
extern ulong thread_max;
extern ulong thread_lost;
extern ulong file_max;
extern ulong file_lost;
extern long file_handle_max;
extern ulong file_handle_lost;
extern ulong table_max;
extern ulong table_lost;
extern ulong socket_max;
extern ulong socket_lost;
extern ulong events_waits_history_per_thread;
extern ulong events_stages_history_per_thread;
extern ulong events_statements_history_per_thread;
extern ulong locker_lost;
extern ulong statement_lost;

/* Exposing the data directly, for iterators. */

extern PFS_mutex *mutex_array;
extern PFS_rwlock *rwlock_array;
extern PFS_cond *cond_array;
extern PFS_thread *thread_array;
extern PFS_file *file_array;
extern PFS_file **file_handle_array;
extern PFS_table *table_array;
extern PFS_socket *socket_array;

void reset_events_waits_by_instance();
void reset_file_instance_io();
void reset_socket_instance_io();

void aggregate_all_event_names(PFS_single_stat *from_array,
                               PFS_single_stat *to_array);
void aggregate_all_event_names(PFS_single_stat *from_array,
                               PFS_single_stat *to_array_1,
                               PFS_single_stat *to_array_2);

void aggregate_all_stages(PFS_stage_stat *from_array,
                          PFS_stage_stat *to_array);

void aggregate_all_statements(PFS_statement_stat *from_array,
                              PFS_statement_stat *to_array);

void aggregate_thread(PFS_thread *thread);
void aggregate_thread_waits(PFS_thread *thread);
void aggregate_thread_stages(PFS_thread *thread);
void aggregate_thread_statements(PFS_thread *thread);

/** Update derived flags for all mutex instances. */
void update_mutex_derived_flags();
/** Update derived flags for all rwlock instances. */
void update_rwlock_derived_flags();
/** Update derived flags for all condition instances. */
void update_cond_derived_flags();
/** Update derived flags for all file handles. */
void update_file_derived_flags();
/** Update derived flags for all table handles. */
void update_table_derived_flags();
/** Update derived flags for all instruments. */
void update_instruments_derived_flags();

/** @} */
#endif

