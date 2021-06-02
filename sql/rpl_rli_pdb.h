/* Copyright (c) 2011, 2020, Oracle and/or its affiliates. All rights reserved.

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

#ifndef RPL_RLI_PDB_H

#define RPL_RLI_PDB_H

#ifdef HAVE_REPLICATION

#include "my_global.h"
#include "my_bitmap.h"         // MY_BITMAP
#include "prealloced_array.h"  // Prealloced_array
#include "log_event.h"         // Format_description_log_event
#include "rpl_mts_submode.h"   // enum_mts_parallel_type
#include "rpl_rli.h"           // Relay_log_info
#include "rpl_slave.h"         // MTS_WORKER_UNDEF

#ifndef DBUG_OFF
extern ulong w_rr;
#endif
/**
  Legends running throughout the module:

  C  - Coordinator
  CP - checkpoint
  W  - Worker

  B-event event that Begins a group (a transaction)
  T-event event that Terminates a group (a transaction)
*/

/* Assigned Partition Hash (APH) entry */
typedef struct st_db_worker_hash_entry
{
  uint  db_len;
  const char *db;
  Slave_worker *worker;
  /*
    The number of transaction pending on this database.
    This should only be modified under the lock slave_worker_hash_lock.
   */
  long usage;
  /*
    The list of temp tables belonging to @ db database is
    attached to an assigned @c worker to become its thd->temporary_tables.
    The list is updated with every ddl incl CREATE, DROP.
    It is removed from the entry and merged to the coordinator's
    thd->temporary_tables in case of events: slave stops, APH oversize.
  */
  TABLE* volatile temporary_tables;

  /* todo: relax concurrency to mimic record-level locking.
     That is to augmenting the entry with mutex/cond pair
     pthread_mutex_t
     pthread_cond_t
     timestamp updated_at; */

} db_worker_hash_entry;

bool init_hash_workers(Relay_log_info *rli);
void destroy_hash_workers(Relay_log_info*);
Slave_worker *map_db_to_worker(const char *dbname, Relay_log_info *rli,
                               db_worker_hash_entry **ptr_entry,
                               bool need_temp_tables, Slave_worker *w);
Slave_worker *get_least_occupied_worker(Relay_log_info *rli,
                                        Slave_worker_array *workers,
                                        Log_event* ev);

#define SLAVE_INIT_DBS_IN_GROUP 4     // initial allocation for CGEP dynarray

#define NUMBER_OF_FIELDS_TO_IDENTIFY_WORKER 2

typedef struct st_slave_job_group
{
  char *group_master_log_name;   // (actually redundant)
  /*
    T-event lop_pos filled by Worker for CheckPoint (CP)
  */
  my_off_t group_master_log_pos;

  /*
     When relay-log name changes  allocates and fill in a new name of relay-log,
     otherwise it fills in NULL.
     Coordinator keeps track of each Worker has been notified on the updating
     to make sure the routine runs once per change.

     W checks the value at commit and memoriezes a not-NULL.
     Freeing unless NULL is left to Coordinator at CP.
  */
  char     *group_relay_log_name; // The value is last seen relay-log
  my_off_t group_relay_log_pos;  // filled by W
  ulong worker_id;
  Slave_worker *worker;
  ulonglong total_seqno;

  my_off_t master_log_pos;       // B-event log_pos
  /* checkpoint coord are reset by periodical and special (Rotate event) CP:s */
  uint  checkpoint_seqno;
  my_off_t checkpoint_log_pos; // T-event lop_pos filled by W for CheckPoint
  char*    checkpoint_log_name;
  my_off_t checkpoint_relay_log_pos; // T-event lop_pos filled by W for CheckPoint
  char*    checkpoint_relay_log_name;
  int32    done;  // Flag raised by W,  read and reset by Coordinator
  ulong    shifted;     // shift the last CP bitmap at receiving a new CP
  time_t   ts;          // Group's timestampt to update Seconds_behind_master
#ifndef DBUG_OFF
  bool     notified;    // to debug group_master_log_name change notification
#endif
  /* Clock-based scheduler requirement: */
  longlong last_committed; // commit parent timestamp
  longlong sequence_number;   // transaction's logical timestamp
  /*
    After Coordinator has seen a new FD event, it sets this member to
    point to the new event, once per worker. Coordinator does so
    when it schedules a first group following the FD event to a worker.
    It checks Slave_worker::fd_change_notified flag to decide whether
    to do this or not.
    When the worker executes the group, it replaces its currently
    active FD by the new FD once it takes on the group first event. It
    checks this member and resets it after the FD replacement is done.

    The member is kind of lock-free. It's updated by Coordinator and
    read by Worker without holding any mutex. That's still safe thanks
    to Slave_worker::jobs_lock that works as synchronizer, Worker
    can't read any stale info.
    The member is updated by Coordinator when it decides which Worker
    an event following a new FD is to be scheduled.
    After Coordinator has chosen a Worker, it queues the event to it
    with necessarily taking Slave_worker::jobs_lock. The Worker grabs
    the mutex lock later at pulling the event from the queue and
    releases the lock before to read from this member.

    This sequence of actions shows the write operation always precedes
    the read one, and ensures no stale FD info is passed to the
    Worker.
  */
  Format_description_log_event *new_fd_event;
  /*
    Coordinator fills the struct with defaults and options at starting of
    a group distribution.
  */
  void reset(my_off_t master_pos, ulonglong seqno)
  {
    master_log_pos= master_pos;
    group_master_log_pos= group_relay_log_pos= 0;
    group_master_log_name= NULL; // todo: remove
    group_relay_log_name= NULL;
    worker_id= MTS_WORKER_UNDEF;
    total_seqno= seqno;
    checkpoint_log_name= NULL;
    checkpoint_log_pos= 0;
    checkpoint_relay_log_name= NULL;
    checkpoint_relay_log_pos= 0;
    checkpoint_seqno= (uint) -1;
    done= 0;
    ts= 0;
#ifndef DBUG_OFF
    notified= false;
#endif
    last_committed= SEQ_UNINIT;
    sequence_number= SEQ_UNINIT;
    new_fd_event= NULL;
  }
} Slave_job_group;

/**
   The class defines a type of queue with a predefined max size that is
   implemented using the circular memory buffer.
   That is items of the queue are accessed as indexed elements of
   the array buffer in a way that when the index value reaches
   a max value it wraps around to point to the first buffer element.
*/
template<typename Element_type>
class circular_buffer_queue
{
public:

  Prealloced_array<Element_type, 1, true> m_Q;
  ulong size;           // the Size of the queue in terms of element
  ulong avail;          // first Available index to append at (next to tail)
  ulong entry;          // the head index or the entry point to the queue.
  volatile ulong len;   // actual length
  bool inited_queue;

  circular_buffer_queue(ulong max) :
    m_Q(PSI_INSTRUMENT_ME),
    size(max), avail(0), entry(max), len(0), inited_queue(false)
  {
    if (!m_Q.reserve(size))
      inited_queue= true;
    m_Q.resize(size);
  }
  circular_buffer_queue() : m_Q(PSI_INSTRUMENT_ME), inited_queue(false) {}
  ~circular_buffer_queue ()
  {
  }

  /**
     Content of the being dequeued item is copied to the arg-pointer
     location.

     @param [out] item A pointer to the being dequeued item.
     @return the queue's array index that the de-queued item
     located at, or
     an error encoded in beyond the index legacy range.
  */
  ulong de_queue(Element_type *item);
  /**
    Similar to de_queue but extracting happens from the tail side.

    @param [out] item A pointer to the being dequeued item.
    @return the queue's array index that the de-queued item
           located at, or an error.
  */
  ulong de_tail(Element_type *item);

  /**
    return the index where the arg item locates
           or an error encoded as a value in beyond of the legacy range
           [0, size) (value `size' is excluded).
  */
  ulong en_queue(Element_type *item);
  /**
     return the value of @c data member of the head of the queue.
  */
  Element_type* head_queue()
  {
    if (empty())
      return NULL;
    return &m_Q[entry];
  }

  bool   gt(ulong i, ulong k); // comparision of ordering of two entities
  /* index is within the valid range */
  bool in(ulong k) { return !empty() &&
      (entry > avail ? (k >= entry || k < avail) : (k >= entry && k < avail)); }
  bool empty() { return entry == size; }
  bool full() { return avail == size; }
};


/**
  Group Assigned Queue whose first element identifies first gap
  in committed sequence. The head of the queue is therefore next to
  the low-water-mark.
*/
class Slave_committed_queue : public circular_buffer_queue<Slave_job_group>
{
public:

  bool inited;

  /* master's Rot-ev exec */
  void update_current_binlog(const char *post_rotate);

  /*
     The last checkpoint time Low-Water-Mark
  */
  Slave_job_group lwm;

  /* last time processed indexes for each worker */
  Prealloced_array<ulonglong, 1> last_done;

  /* the being assigned group index in GAQ */
  ulong assigned_group_index;

  Slave_committed_queue (const char *log, ulong max, uint n)
    : circular_buffer_queue<Slave_job_group>(max), inited(false),
      last_done(key_memory_Slave_job_group_group_relay_log_name)
  {
    if (max >= (ulong) -1 || !inited_queue)
      return;
    else
      inited= TRUE;

    last_done.resize(n);

    lwm.group_relay_log_name=
      (char *) my_malloc(key_memory_Slave_job_group_group_relay_log_name,
                         FN_REFLEN + 1, MYF(0));
    lwm.group_relay_log_name[0]= 0;
    lwm.sequence_number= SEQ_UNINIT;
  }

  ~Slave_committed_queue ()
  {
    if (inited)
    {
      my_free(lwm.group_relay_log_name);
      free_dynamic_items();  // free possibly left allocated strings in GAQ list
    }
  }

#ifndef DBUG_OFF
  bool count_done(Relay_log_info* rli);
#endif

  /* Checkpoint routine refreshes the queue */
  ulong move_queue_head(Slave_worker_array *ws);
  /* Method is for slave shutdown time cleanup */
  void free_dynamic_items();
  /*
     returns a pointer to Slave_job_group struct instance as indexed by arg
     in the circular buffer dyn-array
  */
  Slave_job_group* get_job_group(ulong ind)
  {
    DBUG_ASSERT(ind < size);
    return &m_Q[ind];
  }

  /**
     Assignes @c assigned_group_index to an index of enqueued item
     and returns it.
  */
  ulong en_queue(Slave_job_group *item)
  {
    return assigned_group_index=
      circular_buffer_queue<Slave_job_group>::en_queue(item);
  }

  /**
    Dequeue from head.

    @param [out] item A pointer to the being dequeued item.
    @return The queue's array index that the de-queued item located at,
            or an error encoded in beyond the index legacy range.
  */
  ulong de_queue(Slave_job_group *item)
  {
    return circular_buffer_queue<Slave_job_group>::de_queue(item);
  }

  /**
    Similar to de_queue() but removing an item from the tail side.

    @param [out] item A pointer to the being dequeued item.
    @return the queue's array index that the de-queued item
           located at, or an error.
  */
  ulong de_tail(Slave_job_group *item)
  {
    return circular_buffer_queue<Slave_job_group>::de_tail(item);
  }

  ulong find_lwm(Slave_job_group**, ulong);
};


/**
    @return  the index where the arg item has been located
             or an error.
*/
template <typename Element_type>
ulong circular_buffer_queue<Element_type>::en_queue(Element_type *item)
{
  ulong ret;
  if (avail == size)
  {
    DBUG_ASSERT(avail == m_Q.size());
    return (ulong) -1;
  }

  // store

  ret= avail;
  m_Q[avail]= *item;

  // pre-boundary cond
  if (entry == size)
    entry= avail;

  avail= (avail + 1) % size;
  len++;

  // post-boundary cond
  if (avail == entry)
    avail= size;

  DBUG_ASSERT(avail == entry ||
              len == (avail >= entry) ?
              (avail - entry) : (size + avail - entry));
  DBUG_ASSERT(avail != entry);

  return ret;
}


/**
  Dequeue from head.

  @param [out] item A pointer to the being dequeued item.
  @return the queue's array index that the de-queued item
          located at, or an error as an int outside the legacy
          [0, size) (value `size' is excluded) range.
*/
template <typename Element_type>
ulong circular_buffer_queue<Element_type>::de_queue(Element_type *item)
{
  ulong ret;
  if (entry == size)
  {
    DBUG_ASSERT(len == 0);
    return (ulong) -1;
  }

  ret= entry;
  *item= m_Q[entry];
  len--;

  // pre boundary cond
  if (avail == size)
    avail= entry;
  entry= (entry + 1) % size;

  // post boundary cond
  if (avail == entry)
    entry= size;

  DBUG_ASSERT(entry == size ||
              (len == (avail >= entry)? (avail - entry) :
               (size + avail - entry)));
  DBUG_ASSERT(avail != entry);

  return ret;
}


template <typename Element_type>
ulong circular_buffer_queue<Element_type>::de_tail(Element_type *item)
{
  if (entry == size)
  {
    DBUG_ASSERT(len == 0);
    return (ulong) -1;
  }

  avail= (entry + len - 1) % size;
  *item= m_Q[avail];
  len--;

  // post boundary cond
  if (avail == entry)
    entry= size;

  DBUG_ASSERT(entry == size ||
              (len == (avail >= entry)? (avail - entry) :
               (size + avail - entry)));
  DBUG_ASSERT(avail != entry);

  return avail;
}


class Slave_jobs_queue : public circular_buffer_queue<Slave_job_item>
{
public:
  Slave_jobs_queue() : circular_buffer_queue<Slave_job_item>() {}
  /*
     Coordinator marks with true, Worker signals back at queue back to
     available
  */
  bool overfill;
  ulonglong waited_overfill;
};

class Slave_worker : public Relay_log_info
{
public:
  Slave_worker(Relay_log_info *rli
#ifdef HAVE_PSI_INTERFACE
               ,PSI_mutex_key *param_key_info_run_lock,
               PSI_mutex_key *param_key_info_data_lock,
               PSI_mutex_key *param_key_info_sleep_lock,
               PSI_mutex_key *param_key_info_thd_lock,
               PSI_mutex_key *param_key_info_data_cond,
               PSI_mutex_key *param_key_info_start_cond,
               PSI_mutex_key *param_key_info_stop_cond,
               PSI_mutex_key *param_key_info_sleep_cond
#endif
               , uint param_id, const char *param_channel
              );

  virtual ~Slave_worker();

  Slave_jobs_queue jobs;   // assignment queue containing events to execute
  mysql_mutex_t jobs_lock; // mutex for the jobs queue
  mysql_cond_t  jobs_cond; // condition variable for the jobs queue
  Relay_log_info *c_rli;   // pointer to Coordinator's rli

  Prealloced_array<db_worker_hash_entry*, SLAVE_INIT_DBS_IN_GROUP>
  curr_group_exec_parts; // Current Group Executed Partitions

  bool curr_group_seen_begin; // is set to TRUE with explicit B-event
#ifndef DBUG_OFF
  bool curr_group_seen_sequence_number; // is set to TRUE about starts_group()
#endif
  ulong id;                 // numberic identifier of the Worker

  /*
    Worker runtime statictics
  */
  // the index in GAQ of the last processed group by this Worker
  volatile ulong last_group_done_index;
  ulonglong last_groups_assigned_index; // index of previous group assigned to worker
  ulong wq_empty_waits;  // how many times got idle
  ulong events_done;     // how many events (statements) processed
  ulong groups_done;     // how many groups (transactions) processed
  volatile int curr_jobs; // number of active  assignments
  // number of partitions allocated to the worker at point in time
  long usage_partition;
  // symmetric to rli->mts_end_group_sets_max_dbs
  bool end_group_sets_max_dbs;

  volatile bool relay_log_change_notified; // Coord sets and resets, W can read
  volatile bool checkpoint_notified; // Coord sets and resets, W can read
  volatile bool master_log_change_notified; // Coord sets and resets, W can read
  /*
    The variable serves to Coordinator as a memo to itself
    to notify a Worker about the fact that a new FD has been read.
    Normally, the value is true, to mean the Worker is notified.
    When Coordinator reads a new FD it changes the value to false.
    When Coordinator schedules to a Worker the first event following the new FD,
    it propagates the new FD to the Worker through Slave_job_group::new_fd_event.
    Afterwards Coordinator returns the value back to the regular true,
    to denote things done. Worker will adapt to the new FD once it
    takes on a first event of the marked group.
  */
  bool fd_change_notified;
  ulong bitmap_shifted;  // shift the last bitmap at receiving new CP
  // WQ current excess above the overrun level
  long wq_overrun_cnt;
  /*
    number of events starting from which Worker queue is regarded as
    close to full. The number of the excessive events yields a weight factor
    to compute Coordinator's nap.
  */
  ulong overrun_level;
  /*
     reverse to overrun: the number of events below which Worker is
     considered underruning
  */
  ulong underrun_level;
  /*
    Total of increments done to rli->mts_wq_excess_cnt on behalf of this worker.
    When WQ length is dropped below overrun the counter is reset.
  */
  ulong excess_cnt;
  /*
    Coordinates of the last CheckPoint (CP) this Worker has
    acknowledged; part of is persisent data
  */
  char checkpoint_relay_log_name[FN_REFLEN];
  ulonglong checkpoint_relay_log_pos;
  char checkpoint_master_log_name[FN_REFLEN];
  ulonglong checkpoint_master_log_pos;
  MY_BITMAP group_executed; // bitmap describes groups executed after last CP
  MY_BITMAP group_shifted;  // temporary bitmap to compute group_executed
  ulong checkpoint_seqno;   // the most significant ON bit in group_executed
  /* Initial value of FD-for-execution version until it's gets known. */
  ulong server_version;
  enum en_running_state
  {
    NOT_RUNNING= 0,
    RUNNING= 1,
    ERROR_LEAVING= 2,         // is set by Worker
    STOP= 3,                  // is set by Coordinator upon reciving STOP
    STOP_ACCEPTED= 4          // is set by worker upon completing job when STOP SLAVE is issued
  };

  /*
    This function is used to make a copy of the worker object before we
    destroy it on STOP SLAVE. This new object is then used to report the
    worker status until next START SLAVE following which the new worker objetcs
    will be used.
  */
  void copy_values_for_PFS(ulong worker_id, en_running_state running_status,
                           THD *worker_thd, const Error &last_error,
                           const Gtid_specification &currently_executing_gtid);

  /*
    The running status is guarded by jobs_lock mutex that a writer
    Coordinator or Worker itself needs to hold when write a new value.
  */
  en_running_state volatile running_status;
  /*
    exit_incremented indicates whether worker has contributed to max updated index.
    By default it is set to false. When the worker contibutes for the first time this
    variable is set to true.
  */
  bool exit_incremented;

  int init_worker(Relay_log_info*, ulong);
  int rli_init_info(bool);
  int flush_info(bool force= FALSE);
  static size_t get_number_worker_fields();
  void slave_worker_ends_group(Log_event*, int);
  const char *get_master_log_name();
  ulonglong get_master_log_pos() { return master_log_pos; };
  ulonglong set_master_log_pos(ulong val) { return master_log_pos= val; };
  bool commit_positions(Log_event *evt, Slave_job_group *ptr_g, bool force);
  /*
    When commit fails clear bitmap for executed worker group. Revert back the
    positions to the old positions that existed before commit using the checkpoint.

    @param Slave_job_group a pointer to Slave_job_group struct instance which
    holds group master log pos, group relay log pos and checkpoint positions.
  */
  void rollback_positions(Slave_job_group *ptr_g);
  bool reset_recovery_info();
  /**
    The method runs at Worker initalization, at runtime when
    Coordinator supplied a new FD event for execution context, and at
    the Worker pool shutdown.
    Similarly to the Coordinator's
    Relay_log_info::set_rli_description_event() the possibly existing
    old FD is destoyed, carefully; each worker decrements
    Format_description_log_event::usage_counter and when it is made
    zero the destructor runs.
    Unlike to Coordinator's role, the usage counter of the new FD is *not*
    incremented, see @c Log_event::get_slave_worker() where and why it's done
    there.

    Notice, the method is run as well by Coordinator per each Worker at MTS
    shutdown time.

    Todo: consider to merge logics of the method with that of
    Relay_log_info class.

    @param fdle   pointer to a new Format_description_log_event
  */
  void set_rli_description_event(Format_description_log_event *fdle)
  {
    DBUG_ENTER("Slave_worker::set_rli_description_event");

    if (fdle)
    {
      /*
        When the master rotates its binary log, set gtid_next to
        NOT_YET_DETERMINED.  This tells the slave thread that:

        - If a Gtid_log_event is read subsequently, gtid_next will be
          set to the given GTID (this is done in
          gtid_pre_statement_checks()).

        - If a statement is executed before any Gtid_log_event, then
          gtid_next is set to anonymous (this is done in
          Gtid_log_event::do_apply_event().

        It is imporant to not set GTID_NEXT=NOT_YET_DETERMINED in the
        middle of a transaction.  If that would happen when
        GTID_MODE=ON, the next statement would fail because it
        implicitly sets GTID_NEXT=ANONYMOUS, which is disallowed when
        GTID_MODE=ON.  So then there would be no way to end the
        transaction; any attempt to do so would result in this error.
        (It is not possible for the slave threads to have
        gtid_next.type==AUTOMATIC or UNDEFINED in the middle of a
        transaction, but it is possible for a client thread to have
        gtid_next.type==AUTOMATIC and issue a BINLOG statement
        containing this Format_description_log_event.)
      */
      if (!is_in_group() &&
          (info_thd->variables.gtid_next.type == AUTOMATIC_GROUP ||
           info_thd->variables.gtid_next.type == UNDEFINED_GROUP))
      {
        DBUG_PRINT("info", ("Setting gtid_next.type to NOT_YET_DETERMINED_GROUP"));
        info_thd->variables.gtid_next.set_not_yet_determined();
      }
      adapt_to_master_version_updown(fdle->get_product_version(),
                                     get_master_server_version());
    }
    if (rli_description_event)
    {
      DBUG_ASSERT(rli_description_event->usage_counter.atomic_get() > 0);

      if (rli_description_event->usage_counter.atomic_add(-1) == 1)
      {
        /* The being deleted by Worker FD can't be the latest one */
        DBUG_ASSERT(rli_description_event != c_rli->get_rli_description_event());

        delete rli_description_event;
      }
    }
    rli_description_event= fdle;

    DBUG_VOID_RETURN;
  }

  inline void reset_gaq_index() { gaq_index= c_rli->gaq->size; };
  inline void set_gaq_index(ulong val)
  { 
    if (gaq_index == c_rli->gaq->size)
      gaq_index= val;
  };

  int slave_worker_exec_event(Log_event *ev);
  bool retry_transaction(uint start_relay_number, my_off_t start_relay_pos,
                         uint end_relay_number, my_off_t end_relay_pos);

  bool set_info_search_keys(Rpl_info_handler *to);


  /**
    Get coordinator's RLI. Especially used get the rli from
    a slave thread, like this: thd->rli_slave->get_c_rli();
    thd could be a SQL thread or a worker thread.
  */
   virtual Relay_log_info* get_c_rli()
  {
    return c_rli;
  }

  /**
     return an extension "for channel channel_name"
     for error messages per channel
  */
  const char* get_for_channel_str(bool upper_case= false) const;

  longlong sequence_number()
  {
    Slave_job_group* ptr_g= c_rli->gaq->get_job_group(gaq_index);
    return ptr_g->sequence_number;
  }

  bool found_order_commit_deadlock() { return m_order_commit_deadlock; }
  void report_order_commit_deadlock() { m_order_commit_deadlock= true; }
  /**
    @return either the master server version as extracted from the last
            installed Format_description_log_event, or when it was not
            installed then the slave own server version.
  */
  ulong get_master_server_version()
  {
    return !get_rli_description_event() ? server_version :
      get_rli_description_event()->get_product_version();
  }

  // overridden new and delete operators for 64 byte alignment
  static void* operator new(size_t request);
  static void operator delete(void * ptr);

protected:

  virtual void do_report(loglevel level, int err_code,
                         const char *msg, va_list v_args) const;

private:
  ulong gaq_index;          // GAQ index of the current assignment 
  ulonglong master_log_pos; // event's cached log_pos for possibile error report
  void end_info();
  bool read_info(Rpl_info_handler *from);
  bool write_info(Rpl_info_handler *to);
  bool m_order_commit_deadlock;

  Slave_worker& operator=(const Slave_worker& info);
  Slave_worker(const Slave_worker& info);
  bool worker_sleep(ulong seconds);
  bool read_and_apply_events(uint start_relay_number, my_off_t start_relay_pos,
                             uint end_relay_number, my_off_t end_relay_pos);
  void assign_partition_db(Log_event *ev);

  void reset_order_commit_deadlock() { m_order_commit_deadlock= false; }
public:
  /**
     Returns an array with the expected column numbers of the primary key
     fields of the table repository.
  */
  static const uint *get_table_pk_field_indexes();
  /**
     Returns the index of the Channel_name field of the table repository.
  */
  static uint get_channel_field_index();

  /**
    This class aims to do cleanup for workers in retry_transaction method.
  */
  class Retry_context_sentry {
  public:
    /**
      Constructor to inilizate class objects and flags.
    */
    Retry_context_sentry(Slave_worker& parent);
    /**
       This destructor calls clean() method which performs the cleanup.
    */
    virtual ~Retry_context_sentry();
    /**
       Operator to set the value of m_cleaned_up.

       @param [out] Flag to check for cleanup.
       @return the value of flag for each worker.

    */
    Retry_context_sentry& operator=(bool is_cleaned_up);
    /**
       This method performs the cleanup and resets m_order_commit_deadlock flag.
    */
    void clean();
  private:
    Slave_worker& m_parent;           // Object of enclosed class.
    bool m_is_cleaned_up;             // Flag to check for cleanup.
  };
};

void * head_queue(Slave_jobs_queue *jobs, Slave_job_item *ret);
bool handle_slave_worker_stop(Slave_worker *worker, Slave_job_item *job_item);
bool set_max_updated_index_on_stop(Slave_worker *worker,
                                   Slave_job_item *job_item);

TABLE* mts_move_temp_table_to_entry(TABLE*, THD*, db_worker_hash_entry*);
TABLE* mts_move_temp_tables_to_thd(THD*, TABLE*);
// Auxiliary function
TABLE* mts_move_temp_tables_to_thd(THD*, TABLE*, enum_mts_parallel_type);

bool append_item_to_jobs(slave_job_item *job_item,
                         Slave_worker *w, Relay_log_info *rli);
Slave_job_item* de_queue(Slave_jobs_queue *jobs, Slave_job_item *ret);

inline Slave_worker* get_thd_worker(THD *thd)
{
  return static_cast<Slave_worker *>(thd->rli_slave);
}

#endif // HAVE_REPLICATION
#endif
