--source include/have_debug_sync.inc
--source include/galera_cluster.inc
--source include/count_sessions.inc

#
# Test a transaction within session with wsrep_on set to on/off being aborted
# by TOI transaction
#

# common setup
--connect node_1a, 127.0.0.1, root, , test, $NODE_MYPORT_1
--connection node_1
CREATE TABLE t1(id INT PRIMARY KEY AUTO_INCREMENT, k INT);
INSERT INTO t1(k) VALUES (1),(2),(3),(101),(102),(103);
--eval SET SESSION wsrep_on = $wsrep_on_value;

#
# Case 1: Abort by local TOI when query in IDLE state
#
--connection node_1
BEGIN;
UPDATE t1 SET k=k+1 WHERE id<100;

--connection node_1a
ALTER TABLE t1 ADD KEY(k);

--connection node_1
--error ER_LOCK_DEADLOCK
COMMIT;

#
# Case 2: Abort by local TOI when query in EXEC state
#
--connection node_1
SET DEBUG_SYNC="open_tables_after_open_and_process_table SIGNAL open_tables_after_open_and_process_table.reached WAIT_FOR open_tables_after_open_and_process_table.continue";
--send UPDATE t1 SET k=k+1 WHERE id<100

--connection node_1a
SET DEBUG_SYNC="NOW WAIT_FOR open_tables_after_open_and_process_table.reached";
ALTER TABLE t1 ADD KEY(k);

--connection node_1
--error $expected_error
--reap

# todo: this should not be here
#--error 0,ER_LOCK_DEADLOCK
#COMMIT;

#
# Case 3: Abort by remote TOI when query in IDLE state
#
--connection node_1
BEGIN;
UPDATE t1 SET k=k+1 WHERE id<100;

--connection node_2
ALTER TABLE t1 ADD KEY(k);

--connection node_1
--error ER_LOCK_DEADLOCK
COMMIT;

#
# Case 4: Abort by remote TOI when query in EXEC state
#
--connection node_1
SET DEBUG_SYNC="open_tables_after_open_and_process_table SIGNAL open_tables_after_open_and_process_table.reached WAIT_FOR open_tables_after_open_and_process_table.continue";
--send UPDATE t1 SET k=k+1 WHERE id<100

--connection node_1a
SET DEBUG_SYNC="NOW WAIT_FOR open_tables_after_open_and_process_table.reached";

--connection node_2
ALTER TABLE t1 ADD KEY(k);

--connection node_1
--error $expected_error
--reap

# todo: this should not be here
#--error 0,ER_LOCK_DEADLOCK
#COMMIT;


# cleanup
--disconnect node_1a
--connection node_1
SET SESSION wsrep_on = ON;
DROP TABLE t1;
SET DEBUG_SYNC="RESET";
--source include/wait_until_count_sessions.inc
