-- Copyright (c) 2008, 2023, Oracle and/or its affiliates.
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License, version 2.0,
-- as published by the Free Software Foundation.
--
-- This program is also distributed with certain software (including
-- but not limited to OpenSSL) that is licensed under separate terms,
-- as designated in a particular file or component or in included license
-- documentation.  The authors of MySQL hereby grant you an additional
-- permission to link the program and your derivative works with the
-- separately licensed software that they have included with MySQL.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License, version 2.0, for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

-- This file is for warnings that appear sporadically or are expected without
-- signifying a real problem or are generated by many tests. If the warning
-- is seen only with few tests suppress them within the tests.

delimiter ;

use mtr;

--
-- Create table where testcases can insert patterns to
-- be suppressed
--
-- with PXC we keep this table in MyISAM to avoid replication
-- of suppression added to one node to other nodes of the cluster.
CREATE TABLE test_suppressions (
  pattern VARCHAR(255) NOT NULL
) engine=MyISAM;

--
-- Table of full messages (not patterns), suppressed by a test while it
-- is running.
-- Primary key is guaranteed to be unique because the prefix includes the
-- timestamp, which the server guarantees (!) is unique.
--
-- with PXC we keep this table in MyISAM to avoid replication
-- of suppression added to one node to other nodes of the cluster.
CREATE TABLE asserted_test_suppressions (
  message TEXT NOT NULL,
  PRIMARY KEY(message(100))
) engine=MyISAM;

--
-- Table of patterns for messages that should be excluded from global
-- suppressions. These can be added per test, using
-- include/suppress_messages.inc with
-- $suppress_mode=IGNORE_GLOBAL_SUPPRESSIONS, and will be reset after
-- each test.
--
-- The patterns in this table do not need to be identical to any
-- global suppression pattern. Instead, the pattern should match the
-- error messages for which global suppression should be ignored.
--
-- with PXC we keep this table in MyISAM to avoid replication
-- of suppression added to one node to other nodes of the cluster.
CREATE TABLE test_ignored_global_suppressions (
  pattern VARCHAR(255) NOT NULL,
  PRIMARY KEY(pattern(255))
) engine=MyISAM;

--
-- Declare a trigger that makes sure
-- no invalid patterns can be inserted
-- into test_suppressions
--
SET @character_set_client_saved = @@character_set_client;
SET @character_set_results_saved = @@character_set_results;
SET @collation_connection_saved = @@collation_connection;
SET @@character_set_client = latin1;
SET @@character_set_results = latin1;
SET @@collation_connection = latin1_swedish_ci;

DELIMITER $$

/*!50002
CREATE DEFINER=root@localhost TRIGGER ts_insert
BEFORE INSERT ON test_suppressions
FOR EACH ROW BEGIN
  DECLARE dummy INT;
  SET GLOBAL regexp_time_limit = 0;
  SELECT "" REGEXP NEW.pattern INTO dummy;
  SET GLOBAL regexp_time_limit = DEFAULT;
END
*/;$$

DELIMITER ;

SET @@character_set_client = @character_set_client_saved;
SET @@character_set_results = @character_set_results_saved;
SET @@collation_connection = @collation_connection_saved;


--
-- Load table with patterns that will be suppressed globally(always)
--
CREATE TABLE global_suppressions (
  pattern VARCHAR(255) NOT NULL,
  KEY(pattern(255))
);


-- Declare a trigger that makes sure
-- no invalid patterns can be inserted
-- into global_suppressions
--
SET @character_set_client_saved = @@character_set_client;
SET @character_set_results_saved = @@character_set_results;
SET @collation_connection_saved = @@collation_connection;
SET @@character_set_client = latin1;
SET @@character_set_results = latin1;
SET @@collation_connection = latin1_swedish_ci;

DELIMITER $$

/*!50002
CREATE DEFINER=root@localhost TRIGGER gs_insert
BEFORE INSERT ON global_suppressions
FOR EACH ROW BEGIN
  DECLARE dummy INT;
  SET GLOBAL regexp_time_limit = 0;
  SELECT "" REGEXP NEW.pattern INTO dummy;
  SET GLOBAL regexp_time_limit = DEFAULT;
END
*/;$$

DELIMITER ;

SET @@character_set_client = @character_set_client_saved;
SET @@character_set_results = @character_set_results_saved;
SET @@collation_connection = @collation_connection_saved;



--
-- Insert patterns that should always be suppressed
--
INSERT INTO global_suppressions VALUES
 ("Error reading packet"),
 ("Event Scheduler"),
 ("Forcing close of thread"),

 ("innodb-page-size has been changed"),

 /*
   TODO(tdidriks) Move most of these to individual test cases.
   OS error code 1:  Operation not permitted
   MY-000131 (handler): Command not supported by database
   MY-000155 (handler): The table does not exist in engine
   MY-000160 (handler): There's no partition in table for the given value
   MY-000168 (handler): Unknown (generic) error from engine
   MY-000180 (handler): Index corrupted
   MY-000196 (handler): Query interrupted
   MY-003044 (ER_STD_BAD_ALLOC_ERROR):
                        Memory allocation error: %-.256s in function %s.
  */
 ("Got error 1 when reading table"),
 ("Got error 131 when reading table"),
 ("Got error 155 when reading table"),
 ("Got error 160 when reading table"),
 ("Got error 168 when reading table"),
 ("Got error 180 when reading table"),
 ("Got error 196 when reading table"),
 ("Got error 3044 when reading table"),

 /*
   Due to timing issues, it might be that this warning
   is printed when the server shuts down and the
   computer is loaded.
 */

 ("Lock wait timeout exceeded"),
 ("Log entry on source is longer than max_allowed_packet"),
 ("unknown option '--loose-"),
 ("unknown variable 'loose-"),
 ("Setting lower_case_table_names=2"),
 ("NDB Binlog:"),
 ("Neither --relay-log nor --relay-log-index were used"),
 ("Query partially completed"),
 ("Replica SQL thread is stopped because UNTIL condition"),
 ("Replica SQL thread retried transaction"),
 ("Replica: .*source may suffer from"),
 ("Replica: Table .* doesn't exist"),
 ("Replica: Unknown error.* MY-001105"),
 ("Time-out in NDB"),
 ("You have an error in your SQL syntax"),
 ("deprecated"),
 ("equal MySQL server ids"),
 ("Error .*connecting to source"),
 ("error reading log entry"),
 ("lower_case_table_names is set"),
 ("skip-name-resolve mode"),
 ("replica SQL thread aborted"),
 ("Replica: .*Duplicate entry"),
 /* In certain cases, due to unlucky scheduling, we might receive a temporary
 warning about running out of space in the redo log. In such case, it might
 result in temporary stall and suggestion to increase space in the redo log
 by increasing the innodb_redo_log_capacity. */
 ("Consider increasing innodb_redo_log_capacity."),
 ("Redo log reclaimed some free space"),

 /*
    innodb_dedicated_server warning which raised if innodb_buffer_pool_size,
    innodb_redo_log_capacity or innodb_flush_method is specified.
 */
 ("InnoDB: Option innodb_dedicated_server is ignored"),

/*
  Message seen on debian when built with -DWITH_ASAN=ON
*/
 ("setrlimit could not change the size of core files to 'infinity'"),

 /*It will print a warning if a new UUID of server is generated.*/
 ("No existing UUID has been found, so we assume that this is the first time that this server has been started.*"),
 /*It will print a warning if server is run without --explicit_defaults_for_timestamp.*/
 ("TIMESTAMP with implicit DEFAULT value is deprecated. Please use --explicit_defaults_for_timestamp server option (see documentation for more details)*"),

 /* Added 2009-08-XX after fixing Bug #42408 */

 (": The MySQL server is running with the --secure-backup-file-priv option so it cannot execute this statement"),

 /* Messages from valgrind */
 ("==[0-9]*== Memcheck,"),
 ("==[0-9]*== Copyright"),
 ("==[0-9]*== Using"),
 /* valgrind-3.5.0 dumps this */
 ("==[0-9]*== Command: "),
 /* Messages from valgrind tools */
 ("==[0-9]*== Callgrind"),
 ("==[0-9]*== For interactive control, run 'callgrind_control -h'"),
 ("==[0-9]*== Events    :"),
 ("==[0-9]*== Collected : [0-9]+"),
 ("==[0-9]*== I   refs:      [0-9]+"),
 ("==[0-9]*== Massif"),
 ("==[0-9]*== Helgrind"),

 /* Suppress warnings caused by foreign clients, see Bug#31893901 */

 ("IP address .* could not be resolved.*"),

 /*
   Transient network failures that cause warnings on reconnect.
   BUG#47743 and BUG#47983.
 */
 ("Replica I/O.*: Get source SERVER_UUID failed with error:.*"),
 ("Replica I/O.*: Get source SERVER_ID failed with error:.*"),

 /*
   Warning message is printed out whenever a replica is started with
   a configuration that is not crash-safe.
 */
 (".*If a crash happens this configuration does not guarantee.*"),

 /*
   Warning messages introduced in the context of the WL#4143.
 */
 ("Storing MySQL user name or password information in the connection metadata repository is not secure.*"),
/*
  In MTS if the user issues a stop replica sql while it is scheduling a group
  of events, this warning is emitted.
  */
 ("Replica SQL.*: Coordinator thread of multi-threaded replica is being stopped in the middle of assigning a group of events.*"),
 /*
  Warning messages seen on Fedora and older Debian and Ubuntu versions
 */
 ("Changed limits: max_open_files: *"),
 ("Changed limits: table_open_cache: *"),

 /*
   Warning message introduced by wl#7706
 */
 ("CA certificate .* is self signed"),

 /*
   Warnings related to --secure-file-priv
 */
 ("Insecure configuration for --secure-file-priv:*"),

 /*
   Warnings related to --secure-log-path
 */
 ("Insecure configuration for --secure-log-path:*"),

 /*
   Bug#26585560, warning related to --pid-file
 */
 ("Insecure configuration for --pid-file:*"),
 ("Few location(s) are inaccessible while checking PID filepath"),
 /*
   Following WL#12670, this warning is expected.
 */
 ("Setting named_pipe_full_access_group='\\*everyone\\*' is insecure"),

 /*
   On slow runs (valgrind) the message may be sent twice.
  */
 ("The member with address .* has already sent the stable set. Therefore discarding the second message."),

 /*
   We do have offline members on some Group Replication tests, XCom
   will throw warnings when trying to connect to them.
 */
 ("\\[GCS\\] The member is already leaving or joining a group."),
 ("\\[GCS\\] The member is leaving a group without being on one."),
 ("\\[GCS\\] Processing new view on handler without a valid group configuration."),
 ("\\[GCS\\] Error on opening a connection to .*"),
 ("\\[GCS\\] Error pushing message into group communication engine."),
 ("\\[GCS\\] Message cannot be sent because the member does not belong to a group."),
 ("\\[GCS\\] Automatically adding IPv. localhost address to the allowlist. It is mandatory that it is added."),
 ("\\[GCS\\] Unable to bind to INADDR_ANY:.*"),
 ("\\[GCS\\] Unable to announce tcp port .*. Port already in use\\?"),
 ("\\[GCS\\] Error joining the group while waiting for the network layer to become ready."),
 ("\\[GCS\\] The member was unable to join the group. Local port: .*"),
 ("Shutting down an outgoing connection. This happens because something might be wrong .*"),
 ("Member with address .* has become unreachable."),
 ("This server is not able to reach a majority of members in the group.*"),
 ("Member with address .* is reachable again."),
 ("The member has resumed contact with a majority of the members in the group.*"),
 ("Members removed from the group.*"),
 ("Error while sending message for group replication recovery"),
 ("Replica SQL for channel 'group_replication_recovery': ... The replica coordinator and worker threads are .*"),
 ("A message intended for a client cannot be sent there as no client-session is attached. Therefore, we're sending the information to the error-log instead: MY-001160 - Got an error writing communication packets.*"),
 ("A message intended for a client cannot be sent there as no client-session is attached. Therefore, we're sending the information to the error-log instead: MY-001158 - Got an error reading communication packets.*"),
 ("Failed to establish MySQL client connection in Group Replication.*."),
 ("\\[GCS\\] client closed the signalling connection .*"),
 ("\\[GCS\\] local_server: client closed the signalling connection.*"),
 ("\\[GCS\\] local_server: error reading from the signalling connection.*"),
 ("\\[GCS\\] Unable to start XCom Network Provider.*"),
 ("\\[GCS\\] Error initializing the group communication engine.*"),

 /*
   Warnings/errors related to SSL connection by mysqlx
 */
 ("Plugin mysqlx reported: 'Unable to use user mysql.session account when connecting the server for internal plugin requests.'"),
 ("Plugin mysqlx reported: 'Failed at SSL configuration: \"SSL_CTX_new failed\""),
 ("Plugin mysqlx reported: 'Could not open"),
 ("Plugin mysqlx reported: 'All I/O interfaces are disabled"),
 ("Plugin mysqlx reported: 'Failed at SSL configuration: \"SSL context is not usable without certificate and private key\"'"),

 /*
   Missing Private/Public key files
 */
 ("RSA private key file not found"),
 ("RSA public key file not found"),

 /*
   SSL Library instrumentation failed
 */
 ("The SSL library function CRYPTO_set_mem_functions failed"),

 /*
   binlog-less replica (WL#7846)
 */
 ("The transaction owned GTID is already in the gtid_executed table"),

 /*
   Galera suppressions
 */
 (" *down context*"),
 (" Failed to send state UUID:*"),
 ("wsrep_sst_receive_address is set to '127.0.0.1"),
 ("option --wsrep-causal-reads is deprecated"),
 ("--wsrep-sync-wait=.* takes precedence over --wsrep-causal-reads=OFF"),
 ("--wsrep-causal-reads=ON takes precedence over --wsrep-sync-wait=.*"),
 ("Could not open saved state file for reading: "),
 ("Could not open state file for reading: "),
 ("No persistent state found. Bootstraping with default state"),
 ("Fail to access the file.*gvwstate.*error.*No such file or directory.*"),
 ("access file\\(.*gvwstate\\.dat\\) failed\\(No such file or directory\\)"),
 ("Gap in state sequence\\. Need state transfer\\."),
 ("Failed to prepare for incremental state transfer: Local state UUID \\(00000000-0000-0000-0000-000000000000\\) does not match group state UUID"),
 ("No existing UUID has been found, so we assume that this is the first time that this server has been started\\. Generating a new UUID: "),
 ("last inactive check more than"),
 ("binlog cache not empty \\(0 bytes\\) at connection close"),
 ("SQL statement.*was not replicated.*"),
 ("SQL statement was ineffective"),
 ("Refusing exit for the last slave thread"),
 ("Quorum: No node with complete state"),
 ("Failed to report last committed"),
 ("Slave SQL: Error 'Duplicate entry"),
 ("Query apply warning:"),
 ("Ignoring error for TO isolated action:"),
 ("Initial position was provided by configuration or SST.*"),
 ("Initial position was provided by configuration or SST, avoiding override"),
 ("Warning: Using a password on the command line interface can be insecure"),
 ("InnoDB: Error: Table \"mysql\"\\.\"innodb_table_stats\" not found"),
 ("but it is impossible to select State Transfer donor: Resource temporarily unavailable"),
 ("Could not find peer"),
 ("discarding established \\(time wait\\)"),
 ("sending install message failed: Resource temporarily unavailable"),
 ("Ignoring possible split-brain \\(allowed by configuration\\) from view"),
 ("no nodes coming from prim view, prim not possible"),
 ("Failed to prepare for incremental state transfer: Local state seqno is undefined:"),
 ("gcs_caused\\(\\) returned -107 \\(Transport endpoint is not connected\\)"),
 ("gcs_caused\\(\\) returned -57 \\(Socket is not connected\\)"),
 ("gcs_caused\\(\\) returned -1 \\(Operation not permitted\\)"),
 ("Action message in non-primary configuration from member"),
 ("SYNC message from member"),
 ("InnoDB: Resizing redo log from"),
 ("InnoDB: Starting to delete and rewrite log files"),
 ("InnoDB: New log files created, LSN="),
 ("Transport endpoint is not connected"),
 ("Socket is not connected"),
 ("is not in state transfer"),
 ("JOIN message from member .* in non-primary configuration"),
 ("install timer expired"),
 ("Last Applied Action message in non-primary configuration from member"),

 ("IP address \'127.0.0.2\' could not be resolved: Name or service not known"),
 ("JOIN message from member .* in non-primary configuration"),
 ("SYNC message from member"),
 ("Percona-XtraDB-Cluster prohibits setting binlog_format to STATEMENT or MIXED at global level"),
 ("Table without explict primary key \\(not-recommended\\) and certification of nonPK table is OFF too"),
 ("Node is not a cluster node. Disabling pxc_strict_mode"),
 ("pxc_strict_mode can be changed only if node is cluster-node"),
 ("Toggling wsrep_on to OFF will affect sql_log_bin"),
 ("Toggling wsrep_on to ON will affect sql_log_bin"),
 ("InnoDB High Priority being used"),

 /* MySQL supression needed by Galera */
 ("Slave I/O.*: Get master clock failed with error:.*"),
 ("Trying to access missing tablespace.*"),
 ("Allocated tablespace ID .*, old maximum was.*"),


-- this is not from galera but from PS/MySQL but upstream doesn't need it as the
-- is warning message and warning messages are not printed during mtr.
-- PXC enabled log_verbosity=3 for mtr run so PXC needs it.
 ("Failed to set O_DIRECT "),

 /*
   TLS warnings
 */
 ("Channel mysql_main configured to support TLS"),

 /*
   systemd debug (when built WITH_SYSTEMD and WITH_SYSTEMD_DEBUG,
   and systemd logging is not actually available)
 */
 ("NOTIFY_SOCKET not set in environment. sd_notify messages will not be sent!"),
 ("Invalid systemd notify socket, cannot send: "),

 /*
   Manifest file processing
 */
 ("Manifest file '.*' is not read-only. For better security, please make sure that the file is read-only."),

 /*
   Warnings/errors seen while using group replication with Percona XtraDB Cluster
 */
 ("Using group replication with Percona XtraDB Cluster is only supported for migration"),

 /*
   Warnings from Clone plugin

   In PXC, the table mtr.test_suppressions is created as MyISAM to avoid
   replication of suppression added to one node to other nodes of the cluster.
 */
 ("Non innodb table: mtr.test_suppressions is not cloned and is empty."),
 ("Clone removing all user data for provisioning: Started"),
 ("Clone removing all user data for provisioning: Finished"),
 ("\\[Warning\\] .*Non innodb table: .* is not cloned and is empty."),
 ("\\[ERROR\\] .*MY-\\d+.*clone_check_recovery_crashpoint.*"),

 /*
   Warnings/errors seen when server is loaded with keyring plugin without
   enabling pxc_encrypt_cluster_traffic.
 */
 ("You have enabled keyring plugin. SST encryption is mandatory."),
 ("No suitable '.*' service implementation found"),


 ("THE_LAST_SUPPRESSION");


DELIMITER $$

CREATE DEFINER=root@localhost PROCEDURE filter_global_suppressed_warnings()
BEGIN
  --
  -- Protect the mark on lines that match an 'ignore suppression' pattern.
  --
  SET GLOBAL regexp_time_limit = 0;
  UPDATE error_log el, test_ignored_global_suppressions igs
    SET suspicious = 2 WHERE el.line REGEXP igs.pattern;
  --
  -- Remove the mark from lines that are suppressed by global suppressions.
  --
  UPDATE error_log el, global_suppressions gs
    SET suspicious = 0 WHERE el.suspicious = 1 AND el.line REGEXP gs.pattern;
  --
  -- Un-protect lines that matched an 'ignore suppression' pattern above.
  --
  UPDATE error_log SET suspicious = 1 WHERE suspicious = 2;
  SET GLOBAL regexp_time_limit = DEFAULT;
END$$

CREATE DEFINER=root@localhost PROCEDURE filter_test_suppressed_warnings()
BEGIN
  --
  -- Remove mark from lines that are suppressed by test specific suppressions
  --
  SET GLOBAL regexp_time_limit = 0;
  UPDATE error_log el, test_suppressions ts
    SET suspicious=0
      WHERE el.suspicious=1 AND el.line REGEXP ts.pattern;
  SET GLOBAL regexp_time_limit = DEFAULT;
END$$

CREATE DEFINER=root@localhost PROCEDURE filter_asserted_test_suppressed_warnings()
BEGIN
  --
  -- Remove mark from lines that were expected by assert_error_log.inc
  -- This check is based on string equality, not regex search, so that
  -- we catch cases where the same error is generated in some other
  -- context.
  --
  SET GLOBAL regexp_time_limit = 0;
  UPDATE error_log el, asserted_test_suppressions ats
    SET suspicious=0
      WHERE el.suspicious=1 AND el.line = ats.message;
  SET GLOBAL regexp_time_limit = DEFAULT;
END$$

--
-- Procedure that uses the above created tables to check
-- the servers error log for warnings
--
CREATE DEFINER=root@localhost PROCEDURE check_warnings(OUT result INT)
BEGIN
  -- Don't write these queries to binlog
  SET SQL_LOG_BIN=0;

  CALL filter_global_suppressed_warnings();
  CALL filter_test_suppressed_warnings();
  CALL filter_asserted_test_suppressed_warnings();

  --
  -- Get the number of marked lines and return result
  --
  SELECT /*+SET_VAR(use_secondary_engine=OFF)*/ COUNT(*) INTO @num_warnings FROM error_log
    WHERE suspicious=1;

  IF @num_warnings > 0 THEN
    SELECT /*+SET_VAR(use_secondary_engine=OFF)*/ line
        FROM error_log WHERE suspicious=1;
    -- SELECT * FROM test_suppressions;
    -- Return 2 -> check failed
    SELECT 2 INTO result;
  ELSE
    -- Return 0 -> OK
    SELECT 0 INTO RESULT;
  END IF;

  -- Cleanup for next test
  IF @@wsrep_on = 1 THEN
    -- The TRUNCATE should not be replicated under Galera
    -- as it causes the custom suppressions on the other
    -- nodes to be deleted as well
    SET wsrep_on = 0;
    TRUNCATE test_suppressions;
    TRUNCATE test_ignored_global_suppressions;
    TRUNCATE asserted_test_suppressions;
    SET wsrep_on = 1;
  ELSE 
    TRUNCATE test_suppressions;
    TRUNCATE test_ignored_global_suppressions;
    TRUNCATE asserted_test_suppressions;
  END IF;    

  DROP TABLE error_log;

END$$

--
-- Declare a procedure testcases can use to insert test
-- specific suppressions
--
/*!50001
CREATE DEFINER=root@localhost
PROCEDURE add_suppression(pattern VARCHAR(255))
BEGIN
  INSERT INTO test_suppressions (pattern) VALUES (pattern);
END
*/$$

DELIMITER ;



