/* Copyright (c) 2020, Oracle and/or its affiliates.

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

/**
  @file sql/server_component/log_sink_perfschema.h

  This file contains

  a) The API for the reading of previously written error logs.
  (These functions will in turn use a parse-function defined
  in a log-sink. Whichever log-sink that has a parse-function
  is listed first in @@global.log_error_services will be used;
  that service will decide what log-file to read (i.e. its name)
  and how to parse it. We initially support the reading of JSON-
  formatted error log files and of the traditional MySQL error
  log files.)
  This lets us restore error log information from previous runs
  when the server starts.
  These functions are called from mysqld.cc at start-up.

  b) The log-sink that adds errors logged at run-time to the ring-buffer
  (to be called from @see log_line_submit() during normal operation, i.e.
  when loadable log-components are available, connections are accepted,
  and so on).
*/

#ifndef LOG_SINK_PERFSCHEMA_H
#define LOG_SINK_PERFSCHEMA_H

#include "log_builtins_internal.h"
#include "my_thread_local.h"  // my_thread_id

/* "MY-123456" - 6 digits, "MY-", '\0' */
#define LOG_SINK_PFS_ERROR_CODE_LENGTH 10

/* Currently one of "Repl"/"InnoDB"/"Server" + '\0' */
#define LOG_SINK_PFS_SUBSYS_LENGTH 7

typedef struct _log_sink_pfs_event {
  /** Column ERROR_LOG_TIMESTAMP. Logger should forcibly make these unique. */
  ulonglong m_timestamp;

  /** Column ERROR_LOG_THREAD. */
  ulonglong m_thread_id;  // PFS_key_thread_id uses ulonglong, not my_thread_id

  /** Column ERROR_LOG_PRIO. */
  ulong m_prio;

  /** Column ERROR_LOG_ERROR_CODE. */
  char m_error_code[LOG_SINK_PFS_ERROR_CODE_LENGTH];
  uint m_error_code_length;

  /** Column ERROR_LOG_SUBSYS. */
  char m_subsys[LOG_SINK_PFS_SUBSYS_LENGTH];
  uint m_subsys_length;

  /** Column ERROR_LOG_MESSAGE. */
  uint m_message_length;  //< actual length, not counting trailing '\0'
} log_sink_pfs_event;

/*
  We make these public for SHOW STATUS.
  Everybody else should use the getter functions.

  The timestamp is made available to allow for
  easy checks whether log entries were added since
  the interested part last polled.
  The timestamp is provided as a unique value with
  microsecond precision for that use; to view it in
  a more human-friendly format, use

    SELECT FROM_UNIXTIME(variable_value/1000000)
      FROM global_status WHERE variable_name="Error_log_latest_write";

  Thus if you expect to check for new events with some
  regularity, you could start off with

    SELECT 0 INTO @error_log_last_poll;

  and then poll using something like

    SELECT logged,prio,error_code,subsystem,data
      FROM performance_schema.error_log
      WHERE logged>@error_log_last_poll;

    SELECT FROM_UNIXTIME(variable_value/1000000)
      FROM global_status WHERE variable_name="Error_log_latest_write"
      INTO @error_log_last_poll;

  (Ideally though you'd update @error_log_last_poll from the 'logged'
  field (that is, the timestamp) of the last new row you received.)
*/
extern ulong log_sink_pfs_buffered_bytes;   //< bytes in use (now)
extern ulong log_sink_pfs_buffered_events;  //< events in buffer (now)
extern ulong log_sink_pfs_expired_events;   //< number of expired entries (ever)
extern ulong log_sink_pfs_longest_event;    //< longest event seen (ever)
extern ulonglong
    log_sink_pfs_latest_timestamp;  //< timestamp of most recent write

// The public interface to reading the error-log from the ring-buffer:

/// Acquire a read-lock on the ring-buffer.
void log_sink_pfs_read_start();

///  Release read-lock on ring-buffer.
void log_sink_pfs_read_end();

/**
  Get number of events currently in ring-buffer.
  Caller should hold THR_LOCK_log_perschema when reading this.

  @returns  number of events current in ring-buffer (0..)
*/
size_t log_sink_pfs_event_count();

/**
  Get oldest event still in ring-buffer.
  Caller should hold read-lock on THR_LOCK_log_perfschema when calling this.

  @retval  nullptr    No events in buffer
  @retval  otherwise  Address of oldest event in ring-buffer
*/
log_sink_pfs_event *log_sink_pfs_event_first();

/**
  Get event following the supplied one.
  Caller should hold read-lock on THR_LOCK_log_perfschema when calling this.

  @param   e          Last event the caller was processing.
                      This event should be valid, non-NULL,
                      and should not be a wrap-around marker
                      (m_messages_length == 0).

  @retval  nullptr    No more events in ring-buffer
  @retval  otherwise  Address of the next event in the ring-buffer
*/
log_sink_pfs_event *log_sink_pfs_event_next(log_sink_pfs_event *e);

/**
  Use timestamp to check whether a given event-pointer still points
  to a valid event in the ring-buffer.
  Caller should hold THR_LOCK_log_perfschema when calling this.

  @param   e          Address of event
  @param   logged     unique timestamp of event

  @retval  nullptr    Event no longer exists in ring-buffer
  @retval  otherwise  Address of the event in the ring-buffer
*/
log_sink_pfs_event *log_sink_pfs_event_valid(log_sink_pfs_event *e,
                                             ulonglong logged);

// The public interface to restoring the error-log to the ring-buffer:

/**
  Set up ring-buffer for error-log.

  @retval 0    Success - buffer was allocated.
  @retval !=0  Failure - buffer was not allocated.
*/
int log_error_read_log_init();

/**
  Release error log ring-buffer.

  @retval 0 Success - buffer was released, or did not exist in the first
  place.
*/
int log_error_read_log_exit();

/**
  Restore error log messages from previous shutdown.

  We try restoring from the first (leftmost) of those services
  listed in @@global.log_error_services that have the
  LOG_SERVICE_LOG_PARSER characteristic.

  It is assumed that the last run's log file name is the same
  as the current one's. That is to say, we check whether the
  file supplied to --log-error already exists.

  Once we have determined what file to read from, we'll call
  @see log_error_read_loop() to do the actual reading and parsing.

  @param  log_name  The log file to read (log_error_dest).

  @retval LOG_SERVICE_SUCCESS                 Success (log read and parsed)
  @retval LOG_SERVICE_UNABLE_TO_READ          Could not read/access() file
  @retval LOG_SERVICE_INVALID_ARGUMENT        Invalid file-name (no '.')
  @retval LOG_SERVICE_NOT_AVAILABLE           No log_component active that can
                                              parse log-files
  @retval LOG_SERVICE_ARGUMENT_TOO_LONG       File-name too long
  @retval LOG_SERVICE_COULD_NOT_MAKE_LOG_NAME Could not determine file extension
  @retval otherwise                           Return value from reader
*/
log_service_error log_error_read_log(const char *log_name);

/**
  Add a log-event to the ring buffer.

  In the ring-buffer, each event exists as a header and a blob.
  The header is a log_sink_pfs_event struct containing the
  traditional error-log columns. It is followed by a variable-length
  blob that contains just the message string in traditional log mode,
  and the complete event as JSON in JSON log format.
  If the variable-length is odd, we adjust to the next even boundary.

  If writing the event would go past the end of the ring-buffer,
  we wrap around to the beginning of the buffer.

  @param  e         filled-in event struct to copy into the ring-buffer
  @param  blob_src  variable-length part of the event to add to the ring-buffer

  @retval LOG_SERVICE_SUCCESS
  @retval LOG_SERVICE_INVALID_ARGUMENT
*/
log_service_error log_sink_pfs_event_add(log_sink_pfs_event *e,
                                         const char *blob_src);

/**
  services: log sinks: logging to performance_schema ring-buffer

  Will write timestamp, label, thread-ID, and message to stderr/file.
  If you should not be able to specify a label, one will be generated
  for you from the line's priority field.

  @param           instance             instance handle
  @param           ll                   the log line to write

  @retval          int                  number of added fields, if any
*/
int log_sink_perfschema(void *instance MY_ATTRIBUTE((unused)), log_line *ll);

#endif /* LOG_SINK_PERFSCHEMA_H */
