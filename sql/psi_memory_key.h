/* Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.

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

#ifndef PSI_MEMORY_KEY_INCLUDED
#define PSI_MEMORY_KEY_INCLUDED

/**
  Instrumented memory key.
  To instrument memory, a memory key must be obtained using @c register_memory.
  Using a zero key always disable the instrumentation.
*/

void register_server_memory_keys();

typedef unsigned int PSI_memory_key;

/*
  MAINTAINER: Please keep this list in order, to limit merge collisions.
*/

/*
 These are defined in misc. .cc files, to avoid linkage problems
 for tools like mysqlbinlog.cc and for unit tests.
*/
extern PSI_memory_key key_memory_Filesort_buffer_sort_keys;
extern PSI_memory_key key_memory_Gtid_set_Interval_chunk;
extern PSI_memory_key key_memory_Gtid_set_to_string;
extern PSI_memory_key key_memory_Incident_log_event_message;
extern PSI_memory_key key_memory_NAMED_ILINK_name;
extern PSI_memory_key key_memory_Rows_query_log_event_rows_query;
extern PSI_memory_key key_memory_Sid_map_Node;
extern PSI_memory_key key_memory_String_value;
extern PSI_memory_key key_memory_log_error_loaded_services;
extern PSI_memory_key key_memory_log_error_stack;
extern PSI_memory_key key_memory_log_event;
extern PSI_memory_key key_memory_Gtid_state_group_commit_sidno;

/*
  These are defined in psi_memory_key.cc
 */
extern PSI_memory_key key_memory_DATE_TIME_FORMAT;
extern PSI_memory_key key_memory_DD_column_statistics;
extern PSI_memory_key key_memory_DD_default_values;
extern PSI_memory_key key_memory_DD_import;
extern PSI_memory_key key_memory_DD_String_type;
extern PSI_memory_key key_memory_Event_queue_element_for_exec_names;
extern PSI_memory_key key_memory_Event_scheduler_scheduler_param;
extern PSI_memory_key key_memory_File_query_log_name;
extern PSI_memory_key key_memory_Filesort_info_merge;
extern PSI_memory_key key_memory_Filesort_info_record_pointers;
extern PSI_memory_key key_memory_Gcalc_dyn_list_block;
extern PSI_memory_key key_memory_Geometry_objects_data;
extern PSI_memory_key key_memory_Gis_read_stream_err_msg;
extern PSI_memory_key key_memory_HASH_ROW_ENTRY;
extern PSI_memory_key key_memory_JOIN_CACHE;
extern PSI_memory_key key_memory_JSON;
extern PSI_memory_key key_memory_LOG_POS_COORD;
extern PSI_memory_key key_memory_LOG_name;
extern PSI_memory_key key_memory_MPVIO_EXT_auth_info;
extern PSI_memory_key key_memory_MYSQL_BIN_LOG_basename;
extern PSI_memory_key key_memory_MYSQL_BIN_LOG_index;
extern PSI_memory_key key_memory_MYSQL_LOCK;
extern PSI_memory_key key_memory_MYSQL_LOG_name;
extern PSI_memory_key key_memory_MYSQL_RELAY_LOG_basename;
extern PSI_memory_key key_memory_MYSQL_RELAY_LOG_index;
extern PSI_memory_key key_memory_Mutex_cond_array_Mutex_cond;
extern PSI_memory_key key_memory_NET_buff;
extern PSI_memory_key key_memory_NET_compress_packet;
extern PSI_memory_key key_memory_Owned_gtids_sidno_to_hash;
extern PSI_memory_key key_memory_Owned_gtids_to_string;
extern PSI_memory_key key_memory_PROFILE;
extern PSI_memory_key key_memory_QUICK_RANGE_SELECT_mrr_buf_desc;
extern PSI_memory_key key_memory_Quick_ranges;
extern PSI_memory_key key_memory_READ_INFO;
extern PSI_memory_key key_memory_READ_RECORD_cache;
extern PSI_memory_key key_memory_Relay_log_info_group_relay_log_name;
extern PSI_memory_key key_memory_Row_data_memory_memory;
extern PSI_memory_key key_memory_Rpl_info_file_buffer;
extern PSI_memory_key key_memory_Rpl_info_table;
extern PSI_memory_key key_memory_SLAVE_INFO;
extern PSI_memory_key key_memory_ST_SCHEMA_TABLE;
extern PSI_memory_key key_memory_Security_context;
extern PSI_memory_key key_memory_Slave_applier_json_diff_vector;
extern PSI_memory_key key_memory_Slave_job_group_group_relay_log_name;
extern PSI_memory_key key_memory_Sort_param_tmp_buffer;
extern PSI_memory_key key_memory_Sys_var_charptr_value;
extern PSI_memory_key key_memory_TABLE;
extern PSI_memory_key key_memory_TABLE_RULE_ENT;
extern PSI_memory_key key_memory_TABLE_sort_io_cache;
extern PSI_memory_key key_memory_TC_LOG_MMAP_pages;
extern PSI_memory_key key_memory_THD_Session_sysvar_resource_manager;
extern PSI_memory_key key_memory_THD_Session_tracker;
extern PSI_memory_key key_memory_THD_db;
extern PSI_memory_key key_memory_THD_handler_tables_hash;
extern PSI_memory_key key_memory_THD_variables;
extern PSI_memory_key key_memory_Table_trigger_dispatcher;
extern PSI_memory_key key_memory_Unique_merge_buffer;
extern PSI_memory_key key_memory_Unique_sort_buffer;
extern PSI_memory_key key_memory_User_level_lock;
extern PSI_memory_key key_memory_XID;
extern PSI_memory_key key_memory_XID_STATE;
extern PSI_memory_key key_memory_acl_mem;
extern PSI_memory_key key_memory_acl_memex;
extern PSI_memory_key key_memory_acl_cache;
extern PSI_memory_key key_memory_acl_map_cache;
extern PSI_memory_key key_memory_binlog_cache_mngr;
extern PSI_memory_key key_memory_binlog_pos;
extern PSI_memory_key key_memory_binlog_recover_exec;
extern PSI_memory_key key_memory_binlog_statement_buffer;
extern PSI_memory_key key_memory_binlog_ver_1_event;
extern PSI_memory_key key_memory_bison_stack;
extern PSI_memory_key key_memory_blob_mem_storage;
extern PSI_memory_key key_memory_db_worker_hash_entry;
extern PSI_memory_key key_memory_delegate;
extern PSI_memory_key key_memory_errmsgs;
extern PSI_memory_key key_memory_fill_schema_schemata;
extern PSI_memory_key key_memory_native_functions;
extern PSI_memory_key key_memory_gdl;
extern PSI_memory_key key_memory_get_all_tables;
extern PSI_memory_key key_memory_global_system_variables;
extern PSI_memory_key key_memory_handler_errmsgs;
extern PSI_memory_key key_memory_handlerton;
extern PSI_memory_key key_memory_hash_index_key_buffer;
extern PSI_memory_key key_memory_help;
extern PSI_memory_key key_memory_histograms;
extern PSI_memory_key key_memory_host_cache_hostname;
extern PSI_memory_key key_memory_ignored_db;
extern PSI_memory_key key_memory_locked_table_list;
extern PSI_memory_key key_memory_locked_thread_list;
extern PSI_memory_key key_memory_my_bitmap_map;
extern PSI_memory_key key_memory_my_str_malloc;
extern PSI_memory_key key_memory_opt_bin_logname;
extern PSI_memory_key key_memory_partition_syntax_buffer;
extern PSI_memory_key key_memory_prepared_statement_map;
extern PSI_memory_key key_memory_prepared_statement_main_mem_root;
extern PSI_memory_key key_memory_protocol_rset_root;
extern PSI_memory_key key_memory_prune_partitions_exec;
extern PSI_memory_key key_memory_queue_item;
extern PSI_memory_key key_memory_quick_group_min_max_select_root;
extern PSI_memory_key key_memory_quick_index_merge_root;
extern PSI_memory_key key_memory_quick_range_select_root;
extern PSI_memory_key key_memory_quick_ror_intersect_select_root;
extern PSI_memory_key key_memory_quick_ror_union_select_root;
extern PSI_memory_key key_memory_rpl_filter;
extern PSI_memory_key key_memory_rpl_slave_check_temp_dir;
extern PSI_memory_key key_memory_rpl_slave_command_buffer;
extern PSI_memory_key key_memory_servers;
extern PSI_memory_key key_memory_shared_memory_name;
extern PSI_memory_key key_memory_show_slave_status_io_gtid_set;
extern PSI_memory_key key_memory_sp_head_call_root;
extern PSI_memory_key key_memory_sp_head_execute_root;
extern PSI_memory_key key_memory_sp_head_main_root;
extern PSI_memory_key key_memory_string_iterator;
extern PSI_memory_key key_memory_table_def_memory;
extern PSI_memory_key key_memory_table_mapping_root;
extern PSI_memory_key key_memory_table_share;
extern PSI_memory_key key_memory_table_triggers_list;
extern PSI_memory_key key_memory_test_quick_select_exec;
extern PSI_memory_key key_memory_thd_main_mem_root;
extern PSI_memory_key key_memory_thd_timer;
extern PSI_memory_key key_memory_thd_transactions;
extern PSI_memory_key key_memory_user_conn;
extern PSI_memory_key key_memory_user_var_entry;
extern PSI_memory_key key_memory_user_var_entry_value;
extern PSI_memory_key key_memory_warning_info_warn_root;
extern PSI_memory_key key_memory_sp_cache;
extern PSI_memory_key key_memory_write_set_extraction;
extern PSI_memory_key key_memory_string_service_iterator;

#endif  // PSI_MEMORY_KEY_INCLUDED
