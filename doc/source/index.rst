.. Percona XtraDB Cluster documentation master file, created by
   sphinx-quickstart on Sat Dec  3 13:59:56 2011.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

==========================================
 Percona XtraDB Cluster 5.6 Documentation
==========================================

|Percona XtraDB Cluster| is High Availability and Scalability solution for MySQL Users.

|Percona XtraDB Cluster| provides:
 
 * Synchronous replication. Transaction either committed on all nodes or none.

 * Multi-master replication. You can write to any node.

 * Parallel applying events on slave. Real "parallel replication".

 * Automatic node provisioning.

 * Data consistency. No more unsynchronized slaves.

|Percona XtraDB Cluster| is fully compatible with |MySQL| or |Percona Server| in the following meaning:

 * Data compatibility. |Percona XtraDB Cluster| works with databases created in |MySQL| / |Percona Server|.

 * Application compatibility. There is no or minimal application changes required to start work with |Percona XtraDB Cluster|.


Introduction
============

.. toctree::
   :maxdepth: 1
   :glob:

   intro
   limitation
   errata

Installation
============

.. toctree::
   :maxdepth: 2
   :glob:
   :titlesonly:

   installation
   installation/compile
   upgrading_guide_55_56

Features
========

.. toctree::
   :maxdepth: 1
   :glob:

   features/highavailability
   features/multimaster-replication

User's Manual
=============

.. toctree::
   :maxdepth: 1
   :glob:

   manual/bootstrap
   manual/state_snapshot_transfer
   manual/xtrabackup_sst
   manual/restarting_nodes
   manual/failover
   manual/monitoring
   manual/certification
   manual/threading_model
   manual/gcache_record-set_cache_difference

How-tos
=======

.. toctree::
   :maxdepth: 1
   :glob:

<<<<<<< HEAD
   howtos/centos_howto
   howtos/ubuntu_howto
   howtos/garbd_howto
   howtos/singlebox
   howtos/3nodesec2
   howtos/encrypt-traffic
   howtos/haproxy
   howtos/virt_sandbox
   howtos/bugreport
||||||| merged common ancestors
   flexibility/log_warnings_suppress
   flexibility/improved_memory_engine
   flexibility/max_binlog_files
   flexibility/extended_mysqldump
   flexibility/extended_select_into_outfile
   flexibility/per_query_variable_statement
   flexibility/extended_mysqlbinlog
   flexibility/slowlog_rotation
   flexibility/mysqlbinlog_change_db
   flexibility/csv_engine_mode
   flexibility/online_gtid_deployment
   flexibility/proxy_protocol_support
   flexibility/per_session_server-id
   flexibility/extended_tls_support
   flexibility/compressed_columns
   flexibility/binlogging_replication_improvements

Reliability Improvements
========================

.. toctree::
   :maxdepth: 1
   :glob:

   reliability/log_connection_error
   reliability/innodb_corrupt_table_action
   reliability/show_slave_status_nolock

Management Improvements
=======================

.. toctree::
   :maxdepth: 1
   :glob:

   management/udf_percona_toolkit
   management/innodb_fake_changes
   management/innodb_kill_idle_trx
   management/enforce_engine
   management/utility_user
   management/secure_file_priv_extended
   management/expanded_program_option_modifiers
   management/changed_page_tracking
   management/pam_plugin
   management/innodb_expanded_fast_index_creation
   management/log_archiving
   management/statement_timeout
   management/backup_locks
   management/audit_log_plugin
   management/start_transaction_with_consistent_snapshot
   management/super_read_only
   management/extended_show_grants
   management/ssl-improvement

Diagnostics Improvements
========================

.. toctree::
   :maxdepth: 1
   :glob:

   diagnostics/user_stats
   diagnostics/slow_extended
   diagnostics/innodb_show_status
   diagnostics/innodb_deadlock_count
   diagnostics/mysql_syslog
   diagnostics/show_engines
   diagnostics/innodb_show_lock_names
   diagnostics/process_list
   diagnostics/misc_info_schema_tables
   diagnostics/thread_based_profiling
   diagnostics/scalability_metrics_plugin
   diagnostics/response_time_distribution
   diagnostics/innodb_fragmentation_count

TokuDB
======

.. toctree::
   :maxdepth: 1
   :glob:

   tokudb/tokudb_intro
   tokudb/tokudb_installation
   tokudb/using_tokudb
   tokudb/tokudb_files_and_file_types
   tokudb/tokudb_file_management
   tokudb/tokudb_background_analyze_table
   tokudb/tokudb_variables
   tokudb/tokudb_status_variables
   tokudb/tokudb_troubleshooting
   tokudb/tokudb_performance_schema
   tokudb/toku_backup
   tokudb/tokudb_faq
   tokudb/removing_tokudb
=======
   flexibility/log_warnings_suppress
   flexibility/improved_memory_engine
   flexibility/max_binlog_files
   flexibility/extended_mysqldump
   flexibility/extended_select_into_outfile
   flexibility/per_query_variable_statement
   flexibility/extended_mysqlbinlog
   flexibility/slowlog_rotation
   flexibility/mysqlbinlog_change_db
   flexibility/csv_engine_mode
   flexibility/online_gtid_deployment
   flexibility/proxy_protocol_support
   flexibility/per_session_server-id
   flexibility/extended_tls_support
   flexibility/compressed_columns
   flexibility/binlogging_replication_improvements

Reliability Improvements
========================

.. toctree::
   :maxdepth: 1
   :glob:

   reliability/log_connection_error
   reliability/innodb_corrupt_table_action
   reliability/show_slave_status_nolock

Management Improvements
=======================

.. toctree::
   :maxdepth: 1
   :glob:

   management/udf_percona_toolkit
   management/innodb_fake_changes
   management/innodb_kill_idle_trx
   management/enforce_engine
   management/utility_user
   management/secure_file_priv_extended
   management/expanded_program_option_modifiers
   management/changed_page_tracking
   management/pam_plugin
   management/innodb_expanded_fast_index_creation
   management/log_archiving
   management/statement_timeout
   management/backup_locks
   management/audit_log_plugin
   management/start_transaction_with_consistent_snapshot
   management/super_read_only
   management/extended_show_grants
   management/ssl-improvement

Diagnostics Improvements
========================

.. toctree::
   :maxdepth: 1
   :glob:

   diagnostics/user_stats
   diagnostics/slow_extended
   diagnostics/innodb_show_status
   diagnostics/innodb_deadlock_count
   diagnostics/mysql_syslog
   diagnostics/show_engines
   diagnostics/innodb_show_lock_names
   diagnostics/process_list
   diagnostics/misc_info_schema_tables
   diagnostics/thread_based_profiling
   diagnostics/scalability_metrics_plugin
   diagnostics/response_time_distribution
   diagnostics/innodb_fragmentation_count

TokuDB
======

.. toctree::
   :maxdepth: 1
   :glob:

   tokudb/tokudb_intro
   tokudb/tokudb_installation
   tokudb/using_tokudb
   tokudb/tokudb_files_and_file_types
   tokudb/tokudb_file_management
   tokudb/tokudb_background_analyze_table
   tokudb/tokudb_variables
   tokudb/tokudb_status_variables
   tokudb/tokudb_fractal_tree_indexing
   tokudb/tokudb_troubleshooting
   tokudb/tokudb_performance_schema
   tokudb/toku_backup
   tokudb/tokudb_faq
   tokudb/removing_tokudb
>>>>>>> Percona-Server-5.6.48-88.0

Reference
=========

.. toctree::
   :maxdepth: 1
   :glob:

   release-notes/release-notes_index
   wsrep-status-index
   wsrep-system-index
   wsrep-provider-index
   wsrep-files-index
   faq
   glossary

* :ref:`genindex`
* :ref:`search`

