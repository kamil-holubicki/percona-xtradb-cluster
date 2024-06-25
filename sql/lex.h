#ifndef LEX_INCLUDED
#define LEX_INCLUDED

/* Copyright (c) 2000, 2024, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is designed to work with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have either included with
   the program or referenced in the documentation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/* This file includes all reserved words and functions */

#include "sql/lex_symbol.h"
#include "string_with_len.h"

/* We don't want to include sql_yacc.h into gen_lex_hash */
#ifdef NO_YACC_SYMBOLS
#define SYM_OR_NULL(A) 0
#else

#include "sql/sql_hints.yy.h"
#include "sql/sql_yacc.h"

#define SYM_OR_NULL(A) A
#endif

#define HINT_COMMENT_STARTER "/*+"
#define HINT_COMMENT_TERMINATOR "*/"

#define SYM(T, A) STRING_WITH_LEN(T), SYM_OR_NULL(A), SG_KEYWORDS
#define SYM_FN(T, A) STRING_WITH_LEN(T), SYM_OR_NULL(A), SG_FUNCTIONS
#define SYM_HK(T, A) STRING_WITH_LEN(T), SYM_OR_NULL(A), SG_HINTABLE_KEYWORDS
#define SYM_H(T, A) STRING_WITH_LEN(T), SYM_OR_NULL(A), SG_HINTS

#define SYM_PERCONA(T, A) SYM(T, A)

/*
  Symbols are broken into separated arrays to allow field names with
  same name as functions.
  These are kept sorted for human lookup (the symbols are hashed).

  NOTE! The symbol tables should be the same regardless of what features
  are compiled into the server. Don't add ifdef'ed symbols to the
  lists
*/

static const SYMBOL symbols[] = {
    /*
     Insert new SQL keywords after that commentary (by alphabetical order):
    */
    {SYM("&&", AND_AND_SYM)},
    {SYM("<", LT)},
    {SYM("<=", LE)},
    {SYM("<>", NE)},
    {SYM("!=", NE)},
    {SYM("=", EQ)},
    {SYM(">", GT_SYM)},
    {SYM(">=", GE)},
    {SYM("<<", SHIFT_LEFT)},
    {SYM(">>", SHIFT_RIGHT)},
    {SYM("<=>", EQUAL_SYM)},
    {SYM("ACCESSIBLE", ACCESSIBLE_SYM)},
    {SYM("ACCOUNT", ACCOUNT_SYM)},
    {SYM("ACTION", ACTION)},
    {SYM("ACTIVE", ACTIVE_SYM)},
    {SYM("ADD", ADD)},
    {SYM("ADMIN", ADMIN_SYM)},
    {SYM("AFTER", AFTER_SYM)},
    {SYM("AGAINST", AGAINST)},
    {SYM("AGGREGATE", AGGREGATE_SYM)},
    {SYM("ALL", ALL)},
    {SYM("ALGORITHM", ALGORITHM_SYM)},
    {SYM("ALWAYS", ALWAYS_SYM)},
    {SYM("AND", AND_SYM)},
    {SYM("ANY", ANY_SYM)},
    {SYM("ARRAY", ARRAY_SYM)},
    {SYM("AS", AS)},
    {SYM("ASC", ASC)},
    {SYM("ASCII", ASCII_SYM)},
    {SYM("ASENSITIVE", ASENSITIVE_SYM)},
    {SYM("AT", AT_SYM)},
    {SYM("ATTRIBUTE", ATTRIBUTE_SYM)},
    {SYM("AUTHENTICATION", AUTHENTICATION_SYM)},
    {SYM("AUTO", AUTO_SYM)},
    {SYM("AUTO_INCREMENT", AUTO_INC)},
    {SYM("AUTOEXTEND_SIZE", AUTOEXTEND_SIZE_SYM)},
    {SYM("AVG", AVG_SYM)},
    {SYM("AVG_ROW_LENGTH", AVG_ROW_LENGTH)},
    {SYM("BACKUP", BACKUP_SYM)},
    {SYM("BEFORE", BEFORE_SYM)},
    {SYM("BEGIN", BEGIN_SYM)},
    {SYM("BERNOULLI", BERNOULLI_SYM)},
    {SYM("BETWEEN", BETWEEN_SYM)},
    {SYM("BIGINT", BIGINT_SYM)},
    {SYM("BINARY", BINARY_SYM)},
    {SYM("BINLOG", BINLOG_SYM)},
    {SYM("BIT", BIT_SYM)},
    {SYM("BLOB", BLOB_SYM)},
    {SYM("BLOCK", BLOCK_SYM)},
    {SYM("BOOL", BOOL_SYM)},
    {SYM("BOOLEAN", BOOLEAN_SYM)},
    {SYM("BOTH", BOTH)},
    {SYM("BTREE", BTREE_SYM)},
    {SYM("BUCKETS", BUCKETS_SYM)},
    {SYM("BULK", BULK_SYM)},
    {SYM("BY", BY)},
    {SYM("BYTE", BYTE_SYM)},
    {SYM("CACHE", CACHE_SYM)},
    {SYM("CASCADE", CASCADE)},
    {SYM("CASCADED", CASCADED)},
    {SYM("CASE", CASE_SYM)},
    {SYM("CATALOG_NAME", CATALOG_NAME_SYM)},
    {SYM("CHAIN", CHAIN_SYM)},
    {SYM("CHALLENGE_RESPONSE", CHALLENGE_RESPONSE_SYM)},
    {SYM("CHANGE", CHANGE)},
    {SYM("CHANGED", CHANGED)},
    {SYM("CHANNEL", CHANNEL_SYM)},
    {SYM("CHAR", CHAR_SYM)},
    {SYM("CHARACTER", CHAR_SYM)},
    {SYM("CHARSET", CHARSET)},
    {SYM("CHECKSUM", CHECKSUM_SYM)},
    {SYM("CIPHER", CIPHER_SYM)},
    {SYM("CLASS_ORIGIN", CLASS_ORIGIN_SYM)},
    {SYM("CLIENT", CLIENT_SYM)},
    {SYM_PERCONA("CLIENT_STATISTICS", CLIENT_STATS_SYM)},
    {SYM("CLONE", CLONE_SYM)},
    {SYM("CLOSE", CLOSE_SYM)},
    {SYM_PERCONA("CLUSTERING", CLUSTERING_SYM)},
    {SYM("COALESCE", COALESCE)},
    {SYM("CODE", CODE_SYM)},
    {SYM("COLLATE", COLLATE_SYM)},
    {SYM("COLLATION", COLLATION_SYM)},
    {SYM("COLUMN", COLUMN_SYM)},
    {SYM("COLUMN_FORMAT", COLUMN_FORMAT_SYM)},
    {SYM("COLUMN_NAME", COLUMN_NAME_SYM)},
    {SYM("COLUMNS", COLUMNS)},
    {SYM("COMMENT", COMMENT_SYM)},
    {SYM("COMMIT", COMMIT_SYM)},
    {SYM("COMMITTED", COMMITTED_SYM)},
    {SYM("COMPACT", COMPACT_SYM)},
    {SYM("COMPLETION", COMPLETION_SYM)},
    {SYM("COMPONENT", COMPONENT_SYM)},
    {SYM("COMPRESSION", COMPRESSION_SYM)},
    {SYM("COMPRESSED", COMPRESSED_SYM)},
    {SYM_PERCONA("COMPRESSION_DICTIONARY", COMPRESSION_DICTIONARY_SYM)},
    {SYM("ENCRYPTION", ENCRYPTION_SYM)},
    {SYM("CONCURRENT", CONCURRENT)},
    {SYM("CONDITION", CONDITION_SYM)},
    {SYM("CONNECTION", CONNECTION_SYM)},
    {SYM("CONSISTENT", CONSISTENT_SYM)},
    {SYM("CONSTRAINT", CONSTRAINT)},
    {SYM("CONSTRAINT_CATALOG", CONSTRAINT_CATALOG_SYM)},
    {SYM("CONSTRAINT_NAME", CONSTRAINT_NAME_SYM)},
    {SYM("CONSTRAINT_SCHEMA", CONSTRAINT_SCHEMA_SYM)},
    {SYM("CONTAINS", CONTAINS_SYM)},
    {SYM("CONTEXT", CONTEXT_SYM)},
    {SYM("CONTINUE", CONTINUE_SYM)},
    {SYM("CONVERT", CONVERT_SYM)},
    {SYM("CPU", CPU_SYM)},
    {SYM("CROSS", CROSS)},
    {SYM("CUBE", CUBE_SYM)},
    {SYM("CUME_DIST", CUME_DIST_SYM)},
    {SYM("CURRENT", CURRENT_SYM)},
    {SYM("CURRENT_DATE", CURDATE)},
    {SYM("CURRENT_TIME", CURTIME)},
    {SYM("CURRENT_TIMESTAMP", NOW_SYM)},
    {SYM("CURRENT_USER", CURRENT_USER)},
    {SYM("CURSOR", CURSOR_SYM)},
    {SYM("CURSOR_NAME", CURSOR_NAME_SYM)},
    {SYM("DATA", DATA_SYM)},
    {SYM("DATABASE", DATABASE)},
    {SYM("DATABASES", DATABASES)},
    {SYM("DATAFILE", DATAFILE_SYM)},
    {SYM("DATE", DATE_SYM)},
    {SYM("DATETIME", DATETIME_SYM)},
    {SYM("DAY", DAY_SYM)},
    {SYM("DAY_HOUR", DAY_HOUR_SYM)},
    {SYM("DAY_MICROSECOND", DAY_MICROSECOND_SYM)},
    {SYM("DAY_MINUTE", DAY_MINUTE_SYM)},
    {SYM("DAY_SECOND", DAY_SECOND_SYM)},
    {SYM("DEALLOCATE", DEALLOCATE_SYM)},
    {SYM("DEC", DECIMAL_SYM)},
    {SYM("DECIMAL", DECIMAL_SYM)},
    {SYM("DECLARE", DECLARE_SYM)},
    {SYM("DEFAULT", DEFAULT_SYM)},
    {SYM("DEFAULT_AUTH", DEFAULT_AUTH_SYM)},
    {SYM("DEFINER", DEFINER_SYM)},
    {SYM("DEFINITION", DEFINITION_SYM)},
    {SYM("DELAYED", DELAYED_SYM)},
    {SYM("DELAY_KEY_WRITE", DELAY_KEY_WRITE_SYM)},
    {SYM("DENSE_RANK", DENSE_RANK_SYM)},
    {SYM("DESC", DESC)},
    {SYM("DESCRIBE", DESCRIBE)},
    {SYM("DESCRIPTION", DESCRIPTION_SYM)},
    {SYM("DETERMINISTIC", DETERMINISTIC_SYM)},
    {SYM("DIAGNOSTICS", DIAGNOSTICS_SYM)},
    {SYM("DIRECTORY", DIRECTORY_SYM)},
    {SYM("DISABLE", DISABLE_SYM)},
    {SYM("DISCARD", DISCARD_SYM)},
    {SYM("DISK", DISK_SYM)},
    {SYM("DISTINCT", DISTINCT)},
    {SYM("DISTINCTROW", DISTINCT)}, /* Access likes this */
    {SYM("DIV", DIV_SYM)},
    {SYM("DO", DO_SYM)},
    {SYM("DOUBLE", DOUBLE_SYM)},
    {SYM("DROP", DROP)},
    {SYM("DUAL", DUAL_SYM)},
    {SYM("DUMPFILE", DUMPFILE)},
    {SYM("DUPLICATE", DUPLICATE_SYM)},
    {SYM("DYNAMIC", DYNAMIC_SYM)},
    {SYM("EACH", EACH_SYM)},
    {SYM_PERCONA("EFFECTIVE", EFFECTIVE_SYM)},
    {SYM("ELSE", ELSE)},
    {SYM("ELSEIF", ELSEIF_SYM)},
    {SYM("EMPTY", EMPTY_SYM)},
    {SYM("ENABLE", ENABLE_SYM)},
    {SYM("ENCLOSED", ENCLOSED)},
    {SYM("END", END)},
    {SYM("ENDS", ENDS_SYM)},
    {SYM("ENFORCED", ENFORCED_SYM)},
    {SYM("ENGINE", ENGINE_SYM)},
    {SYM("ENGINE_ATTRIBUTE", ENGINE_ATTRIBUTE_SYM)},
    {SYM("ENGINES", ENGINES_SYM)},
    {SYM("ENUM", ENUM_SYM)},
    {SYM("ERROR", ERROR_SYM)},
    {SYM("ERRORS", ERRORS)},
    {SYM("ESCAPE", ESCAPE_SYM)},
    {SYM("ESCAPED", ESCAPED)},
    {SYM("EVENT", EVENT_SYM)},
    {SYM("EVENTS", EVENTS_SYM)},
    {SYM("EVERY", EVERY_SYM)},
    {SYM("EXCEPT", EXCEPT_SYM)},
    {SYM("EXCHANGE", EXCHANGE_SYM)},
    {SYM("EXCLUDE", EXCLUDE_SYM)},
    {SYM("EXECUTE", EXECUTE_SYM)},
    {SYM("EXISTS", EXISTS)},
    {SYM("EXIT", EXIT_SYM)},
    {SYM("EXPANSION", EXPANSION_SYM)},
    {SYM("EXPORT", EXPORT_SYM)},
    {SYM("EXPIRE", EXPIRE_SYM)},
    {SYM("EXPLAIN", DESCRIBE)},
    {SYM("EXTENDED", EXTENDED_SYM)},
    {SYM("EXTENT_SIZE", EXTENT_SIZE_SYM)},
    {SYM("FACTOR", FACTOR_SYM)},
    {SYM("FAILED_LOGIN_ATTEMPTS", FAILED_LOGIN_ATTEMPTS_SYM)},
    {SYM("FALSE", FALSE_SYM)},
    {SYM("FAST", FAST_SYM)},
    {SYM("FAULTS", FAULTS_SYM)},
    {SYM("FETCH", FETCH_SYM)},
    {SYM("FIELDS", COLUMNS)},
    {SYM("FILE", FILE_SYM)},
    {SYM("FILE_BLOCK_SIZE", FILE_BLOCK_SIZE_SYM)},
    {SYM("FILTER", FILTER_SYM)},
    {SYM("FINISH", FINISH_SYM)},
    {SYM("FIRST", FIRST_SYM)},
    {SYM("FIRST_VALUE", FIRST_VALUE_SYM)},
    {SYM("FIXED", FIXED_SYM)},
    {SYM("FLOAT", FLOAT_SYM)},
    {SYM("FLOAT4", FLOAT_SYM)},
    {SYM("FLOAT8", DOUBLE_SYM)},
    {SYM("FLUSH", FLUSH_SYM)},
    {SYM("FOLLOWS", FOLLOWS_SYM)},
    {SYM("FOLLOWING", FOLLOWING_SYM)},
    {SYM("FOR", FOR_SYM)},
    {SYM("FORCE", FORCE_SYM)},
    {SYM("FOREIGN", FOREIGN)},
    {SYM("FORMAT", FORMAT_SYM)},
    {SYM("FOUND", FOUND_SYM)},
    {SYM("FROM", FROM)},
    {SYM("FULL", FULL)},
    {SYM("FULLTEXT", FULLTEXT_SYM)},
    {SYM("FUNCTION", FUNCTION_SYM)},
    {SYM("GENERAL", GENERAL)},
    {SYM("GROUP_REPLICATION", GROUP_REPLICATION)},
    {SYM("GEOMCOLLECTION", GEOMETRYCOLLECTION_SYM)},
    {SYM("GEOMETRY", GEOMETRY_SYM)},
    {SYM("GEOMETRYCOLLECTION", GEOMETRYCOLLECTION_SYM)},
    {SYM("GET_FORMAT", GET_FORMAT)},
    {SYM("GET_SOURCE_PUBLIC_KEY", GET_SOURCE_PUBLIC_KEY_SYM)},
    {SYM("GET", GET_SYM)},
    {SYM("GENERATE", GENERATE_SYM)},
    {SYM("GENERATED", GENERATED)},
    {SYM("GLOBAL", GLOBAL_SYM)},
    {SYM("GRANT", GRANT)},
    {SYM("GRANTS", GRANTS)},
    {SYM("GROUP", GROUP_SYM)},
    {SYM("GROUPING", GROUPING_SYM)},
    {SYM("GROUPS", GROUPS_SYM)},
    {SYM("GTIDS", GTIDS_SYM)},
    {SYM("GTID_ONLY", GTID_ONLY_SYM)},
    {SYM("HANDLER", HANDLER_SYM)},
    {SYM("HASH", HASH_SYM)},
    {SYM("HAVING", HAVING)},
    {SYM("HELP", HELP_SYM)},
    {SYM("HIGH_PRIORITY", HIGH_PRIORITY)},
    {SYM("HISTOGRAM", HISTOGRAM_SYM)},
    {SYM("HISTORY", HISTORY_SYM)},
    {SYM("HOST", HOST_SYM)},
    {SYM("HOSTS", HOSTS_SYM)},
    {SYM("HOUR", HOUR_SYM)},
    {SYM("HOUR_MICROSECOND", HOUR_MICROSECOND_SYM)},
    {SYM("HOUR_MINUTE", HOUR_MINUTE_SYM)},
    {SYM("HOUR_SECOND", HOUR_SECOND_SYM)},
    {SYM("IDENTIFIED", IDENTIFIED_SYM)},
    {SYM("IF", IF)},
    {SYM("IGNORE", IGNORE_SYM)},
    {SYM("IGNORE_SERVER_IDS", IGNORE_SERVER_IDS_SYM)},
    {SYM("IMPORT", IMPORT)},
    {SYM("IN", IN_SYM)},
    {SYM("INACTIVE", INACTIVE_SYM)},
    {SYM("INDEX", INDEX_SYM)},
    {SYM_PERCONA("INDEX_STATISTICS", INDEX_STATS_SYM)},
    {SYM("INDEXES", INDEXES)},
    {SYM("INFILE", INFILE_SYM)},
    {SYM("INITIAL", INITIAL_SYM)},
    {SYM("INITIAL_SIZE", INITIAL_SIZE_SYM)},
    {SYM("INITIATE", INITIATE_SYM)},
    {SYM("INNER", INNER_SYM)},
    {SYM("INOUT", INOUT_SYM)},
    {SYM("INSENSITIVE", INSENSITIVE_SYM)},
    {SYM("INSERT_METHOD", INSERT_METHOD)},
    {SYM("INSTALL", INSTALL_SYM)},
    {SYM("INSTANCE", INSTANCE_SYM)},
    {SYM("INT", INT_SYM)},
    {SYM("INT1", TINYINT_SYM)},
    {SYM("INT2", SMALLINT_SYM)},
    {SYM("INT3", MEDIUMINT_SYM)},
    {SYM("INT4", INT_SYM)},
    {SYM("INT8", BIGINT_SYM)},
    {SYM("INTEGER", INT_SYM)},
    {SYM("INTERSECT", INTERSECT_SYM)},
    {SYM("INTERVAL", INTERVAL_SYM)},
    {SYM("INTO", INTO)},
    {SYM("IO", IO_SYM)},
    {SYM("IO_AFTER_GTIDS", IO_AFTER_GTIDS)},
    {SYM("IO_BEFORE_GTIDS", IO_BEFORE_GTIDS)},
    {SYM("IO_THREAD", RELAY_THREAD)},
    {SYM("IPC", IPC_SYM)},
    {SYM("IS", IS)},
    {SYM("ISOLATION", ISOLATION)},
    {SYM("ISSUER", ISSUER_SYM)},
    {SYM("ITERATE", ITERATE_SYM)},
    {SYM("INVISIBLE", INVISIBLE_SYM)},
    {SYM("INVOKER", INVOKER_SYM)},
    {SYM("JOIN", JOIN_SYM)},
    {SYM("JSON", JSON_SYM)},
    {SYM("JSON_TABLE", JSON_TABLE_SYM)},
    {SYM("JSON_VALUE", JSON_VALUE_SYM)},
    {SYM("KEY", KEY_SYM)},
    {SYM("KEYRING", KEYRING_SYM)},
    {SYM("KEYS", KEYS)},
    {SYM("KEY_BLOCK_SIZE", KEY_BLOCK_SIZE)},
    {SYM("KILL", KILL_SYM)},
    {SYM("LAG", LAG_SYM)},
    {SYM("LANGUAGE", LANGUAGE_SYM)},
    {SYM("LAST", LAST_SYM)},
    {SYM("LAST_VALUE", LAST_VALUE_SYM)},
    {SYM("LATERAL", LATERAL_SYM)},
    {SYM("LEAD", LEAD_SYM)},
    {SYM("LEADING", LEADING)},
    {SYM("LEAVE", LEAVE_SYM)},
    {SYM("LEAVES", LEAVES)},
    {SYM("LEFT", LEFT)},
    {SYM("LESS", LESS_SYM)},
    {SYM("LEVEL", LEVEL_SYM)},
    {SYM("LIKE", LIKE)},
    {SYM("LIMIT", LIMIT)},
    {SYM("LINEAR", LINEAR_SYM)},
    {SYM("LINES", LINES)},
    {SYM("LINESTRING", LINESTRING_SYM)},
    {SYM("LIST", LIST_SYM)},
    {SYM("LOCAL", LOCAL_SYM)},
    {SYM("LOCALTIME", NOW_SYM)},
    {SYM("LOCALTIMESTAMP", NOW_SYM)},
    {SYM("LOCK", LOCK_SYM)},
    {SYM("LOCKED", LOCKED_SYM)},
    {SYM("LOCKS", LOCKS_SYM)},
    {SYM("LOGFILE", LOGFILE_SYM)},
    {SYM("LOGS", LOGS_SYM)},
    {SYM("LOG", LOG_SYM)},
    {SYM("LONG", LONG_SYM)},
    {SYM("LONGBLOB", LONGBLOB_SYM)},
    {SYM("LONGTEXT", LONGTEXT_SYM)},
    {SYM("LOOP", LOOP_SYM)},
    {SYM("LOW_PRIORITY", LOW_PRIORITY)},
    {SYM("MANUAL", MANUAL_SYM)},
    {SYM("MASTER", MASTER_SYM)},
    {SYM("MATCH", MATCH)},
    {SYM("MAX_CONNECTIONS_PER_HOUR", MAX_CONNECTIONS_PER_HOUR)},
    {SYM("MAX_QUERIES_PER_HOUR", MAX_QUERIES_PER_HOUR)},
    {SYM("MAX_ROWS", MAX_ROWS)},
    {SYM("MAX_SIZE", MAX_SIZE_SYM)},
    {SYM("MAX_UPDATES_PER_HOUR", MAX_UPDATES_PER_HOUR)},
    {SYM("MAX_USER_CONNECTIONS", MAX_USER_CONNECTIONS_SYM)},
    {SYM("MAXVALUE", MAX_VALUE_SYM)},
    {SYM("MEDIUM", MEDIUM_SYM)},
    {SYM("MEDIUMBLOB", MEDIUMBLOB_SYM)},
    {SYM("MEDIUMINT", MEDIUMINT_SYM)},
    {SYM("MEDIUMTEXT", MEDIUMTEXT_SYM)},
    {SYM("MEMBER", MEMBER_SYM)},
    {SYM("MEMORY", MEMORY_SYM)},
    {SYM("MERGE", MERGE_SYM)},
    {SYM("MESSAGE_TEXT", MESSAGE_TEXT_SYM)},
    {SYM("MICROSECOND", MICROSECOND_SYM)},
    {SYM("MIDDLEINT", MEDIUMINT_SYM)}, /* For powerbuilder */
    {SYM("MIGRATE", MIGRATE_SYM)},
    {SYM("MINUTE", MINUTE_SYM)},
    {SYM("MINUTE_MICROSECOND", MINUTE_MICROSECOND_SYM)},
    {SYM("MINUTE_SECOND", MINUTE_SECOND_SYM)},
    {SYM("MIN_ROWS", MIN_ROWS)},
    {SYM("MOD", MOD_SYM)},
    {SYM("MODE", MODE_SYM)},
    {SYM("MODIFIES", MODIFIES_SYM)},
    {SYM("MODIFY", MODIFY_SYM)},
    {SYM("MONTH", MONTH_SYM)},
    {SYM("MULTILINESTRING", MULTILINESTRING_SYM)},
    {SYM("MULTIPOINT", MULTIPOINT_SYM)},
    {SYM("MULTIPOLYGON", MULTIPOLYGON_SYM)},
    {SYM("MUTEX", MUTEX_SYM)},
    {SYM("MYSQL_ERRNO", MYSQL_ERRNO_SYM)},
    {SYM("NAME", NAME_SYM)},
    {SYM("NAMES", NAMES_SYM)},
    {SYM("NATIONAL", NATIONAL_SYM)},
    {SYM("NATURAL", NATURAL)},
    {SYM("NDB", NDBCLUSTER_SYM)},
    {SYM("NDBCLUSTER", NDBCLUSTER_SYM)},
    {SYM("NCHAR", NCHAR_SYM)},
    {SYM("NESTED", NESTED_SYM)},
    {SYM("NETWORK_NAMESPACE", NETWORK_NAMESPACE_SYM)},
    {SYM("NEVER", NEVER_SYM)},
    {SYM("NEW", NEW_SYM)},
    {SYM("NEXT", NEXT_SYM)},
    {SYM("NO", NO_SYM)},
    {SYM("NO_WAIT", NO_WAIT_SYM)},
    {SYM("NOWAIT", NOWAIT_SYM)},
    {SYM("NODEGROUP", NODEGROUP_SYM)},
    {SYM("NONE", NONE_SYM)},
    {SYM("NOT", NOT_SYM)},
    {SYM("NO_WRITE_TO_BINLOG", NO_WRITE_TO_BINLOG)},
    {SYM("NTH_VALUE", NTH_VALUE_SYM)},
    {SYM("NTILE", NTILE_SYM)},
    {SYM("NULL", NULL_SYM)},
    {SYM("NULLS", NULLS_SYM)},
    {SYM("NUMBER", NUMBER_SYM)},
    {SYM("NUMERIC", NUMERIC_SYM)},
    {SYM("NVARCHAR", NVARCHAR_SYM)},
    {SYM("OF", OF_SYM)},
    {SYM("OFF", OFF_SYM)},
    {SYM("OFFSET", OFFSET_SYM)},
    {SYM("OJ", OJ_SYM)},
    {SYM("OLD", OLD_SYM)},
    {SYM("ON", ON_SYM)},
    {SYM("ONE", ONE_SYM)},
    {SYM("ONLY", ONLY_SYM)},
    {SYM("OPEN", OPEN_SYM)},
    {SYM("OPTIMIZER_COSTS", OPTIMIZER_COSTS_SYM)},
    {SYM("OPTIONS", OPTIONS_SYM)},
    {SYM("OPTION", OPTION)},
    {SYM("OPTIONAL", OPTIONAL_SYM)},
    {SYM("OPTIONALLY", OPTIONALLY)},
    {SYM("OR", OR_SYM)},
    {SYM("ORGANIZATION", ORGANIZATION_SYM)},
    {SYM("OTHERS", OTHERS_SYM)},
    {SYM("ORDER", ORDER_SYM)},
    {SYM("ORDINALITY", ORDINALITY_SYM)},
    {SYM("OUT", OUT_SYM)},
    {SYM("OUTER", OUTER_SYM)},
    {SYM("OUTFILE", OUTFILE)},
    {SYM("OVER", OVER_SYM)},
    {SYM("OWNER", OWNER_SYM)},
    {SYM("PACK_KEYS", PACK_KEYS_SYM)},
    {SYM("PATH", PATH_SYM)},
    {SYM("PARSE_TREE", PARSE_TREE_SYM)},
    {SYM("PARSER", PARSER_SYM)},
    {SYM("PAGE", PAGE_SYM)},
    {SYM("PARALLEL", PARALLEL_SYM)},
    {SYM("PARTIAL", PARTIAL)},
    {SYM("PARTITION", PARTITION_SYM)},
    {SYM("PARTITIONING", PARTITIONING_SYM)},
    {SYM("PARTITIONS", PARTITIONS_SYM)},
    {SYM("PASSWORD", PASSWORD)},
    {SYM("PASSWORD_LOCK_TIME", PASSWORD_LOCK_TIME_SYM)},
    {SYM("PERCENT_RANK", PERCENT_RANK_SYM)},
    {SYM_PERCONA("PERCONA_SEQUENCE_TABLE", PERCONA_SEQUENCE_TABLE_SYM)},
    {SYM("PERSIST", PERSIST_SYM)},
    {SYM("PERSIST_ONLY", PERSIST_ONLY_SYM)},
    {SYM("PHASE", PHASE_SYM)},
    {SYM("PLUGIN", PLUGIN_SYM)},
    {SYM("PLUGINS", PLUGINS_SYM)},
    {SYM("PLUGIN_DIR", PLUGIN_DIR_SYM)},
    {SYM("POINT", POINT_SYM)},
    {SYM("POLYGON", POLYGON_SYM)},
    {SYM("PORT", PORT_SYM)},
    {SYM("PRECEDES", PRECEDES_SYM)},
    {SYM("PRECEDING", PRECEDING_SYM)},
    {SYM("PRECISION", PRECISION)},
    {SYM("PREPARE", PREPARE_SYM)},
    {SYM("PRESERVE", PRESERVE_SYM)},
    {SYM("PREV", PREV_SYM)},
    {SYM("PRIMARY", PRIMARY_SYM)},
    {SYM("PRIVILEGES", PRIVILEGES)},
    {SYM("PRIVILEGE_CHECKS_USER", PRIVILEGE_CHECKS_USER_SYM)},
    {SYM("PROCEDURE", PROCEDURE_SYM)},
    {SYM("PROCESS", PROCESS)},
    {SYM("PROCESSLIST", PROCESSLIST_SYM)},
    {SYM("PROFILE", PROFILE_SYM)},
    {SYM("PROFILES", PROFILES_SYM)},
    {SYM("PROXY", PROXY_SYM)},
    {SYM("PURGE", PURGE)},
    {SYM("QUALIFY", QUALIFY_SYM)},
    {SYM("QUARTER", QUARTER_SYM)},
    {SYM("QUERY", QUERY_SYM)},
    {SYM("QUICK", QUICK)},
    {SYM("RANDOM", RANDOM_SYM)},
    {SYM("RANK", RANK_SYM)},
    {SYM("RANGE", RANGE_SYM)},
    {SYM("READ", READ_SYM)},
    {SYM("READ_ONLY", READ_ONLY_SYM)},
    {SYM("READ_WRITE", READ_WRITE_SYM)},
    {SYM("READS", READS_SYM)},
    {SYM("REAL", REAL_SYM)},
    {SYM("REBUILD", REBUILD_SYM)},
    {SYM("RECOVER", RECOVER_SYM)},
    {SYM("RECURSIVE", RECURSIVE_SYM)},
    {SYM("REDO_BUFFER_SIZE", REDO_BUFFER_SIZE_SYM)},
    {SYM("REDUNDANT", REDUNDANT_SYM)},
    {SYM("REFERENCE", REFERENCE_SYM)},
    {SYM("REFERENCES", REFERENCES)},
    {SYM("REGEXP", REGEXP)},
    {SYM("REGISTRATION", REGISTRATION_SYM)},
    {SYM("RELAY", RELAY)},
    {SYM("RELAYLOG", RELAYLOG_SYM)},
    {SYM("RELAY_LOG_FILE", RELAY_LOG_FILE_SYM)},
    {SYM("RELAY_LOG_POS", RELAY_LOG_POS_SYM)},
    {SYM("RELAY_THREAD", RELAY_THREAD)},
    {SYM("RELEASE", RELEASE_SYM)},
    {SYM("RELOAD", RELOAD)},
    {SYM("REMOVE", REMOVE_SYM)},
    {SYM("RENAME", RENAME)},
    {SYM("ASSIGN_GTIDS_TO_ANONYMOUS_TRANSACTIONS",
         ASSIGN_GTIDS_TO_ANONYMOUS_TRANSACTIONS_SYM)},
    {SYM("REORGANIZE", REORGANIZE_SYM)},
    {SYM("REPAIR", REPAIR)},
    {SYM("REPEATABLE", REPEATABLE_SYM)},
    {SYM("REPLICA", REPLICA_SYM)},
    {SYM("REPLICAS", REPLICAS_SYM)},
    {SYM("REPLICATION", REPLICATION)},
    {SYM("REPLICATE_DO_DB", REPLICATE_DO_DB)},
    {SYM("REPLICATE_IGNORE_DB", REPLICATE_IGNORE_DB)},
    {SYM("REPLICATE_DO_TABLE", REPLICATE_DO_TABLE)},
    {SYM("REPLICATE_IGNORE_TABLE", REPLICATE_IGNORE_TABLE)},
    {SYM("REPLICATE_WILD_DO_TABLE", REPLICATE_WILD_DO_TABLE)},
    {SYM("REPLICATE_WILD_IGNORE_TABLE", REPLICATE_WILD_IGNORE_TABLE)},
    {SYM("REPLICATE_REWRITE_DB", REPLICATE_REWRITE_DB)},
    {SYM("REPEAT", REPEAT_SYM)},
    {SYM("REQUIRE", REQUIRE_SYM)},
    {SYM("REQUIRE_ROW_FORMAT", REQUIRE_ROW_FORMAT_SYM)},
    {SYM("REQUIRE_TABLE_PRIMARY_KEY_CHECK",
         REQUIRE_TABLE_PRIMARY_KEY_CHECK_SYM)},
    {SYM("RESET", RESET_SYM)},
    {SYM("RESPECT", RESPECT_SYM)},
    {SYM("RESIGNAL", RESIGNAL_SYM)},
    {SYM("RESOURCE", RESOURCE_SYM)},
    {SYM("RESTART", RESTART_SYM)},
    {SYM("RESTORE", RESTORE_SYM)},
    {SYM("RESTRICT", RESTRICT)},
    {SYM("RESUME", RESUME_SYM)},
    {SYM("RETAIN", RETAIN_SYM)},
    {SYM("RETURNED_SQLSTATE", RETURNED_SQLSTATE_SYM)},
    {SYM("RETURN", RETURN_SYM)},
    {SYM("RETURNING", RETURNING_SYM)},
    {SYM("RETURNS", RETURNS_SYM)},
    {SYM("REUSE", REUSE_SYM)},
    {SYM("REVERSE", REVERSE_SYM)},
    {SYM("REVOKE", REVOKE)},
    {SYM("RIGHT", RIGHT)},
    {SYM("RLIKE", REGEXP)}, /* Like in mSQL2 */
    {SYM("ROLE", ROLE_SYM)},
    {SYM("ROLLBACK", ROLLBACK_SYM)},
    {SYM("ROLLUP", ROLLUP_SYM)},
    {SYM("ROUTINE", ROUTINE_SYM)},
    {SYM("ROTATE", ROTATE_SYM)},
    {SYM("ROW", ROW_SYM)},
    {SYM("ROW_COUNT", ROW_COUNT_SYM)},
    {SYM("ROW_NUMBER", ROW_NUMBER_SYM)},
    {SYM("ROWS", ROWS_SYM)},
    {SYM("ROW_FORMAT", ROW_FORMAT_SYM)},
    {SYM("RTREE", RTREE_SYM)},
    {SYM("S3", S3_SYM)},
    {SYM("SAVEPOINT", SAVEPOINT_SYM)},
    {SYM("SCHEDULE", SCHEDULE_SYM)},
    {SYM("SCHEMA", DATABASE)},
    {SYM("SCHEMA_NAME", SCHEMA_NAME_SYM)},
    {SYM("SCHEMAS", DATABASES)},
    {SYM("SECOND", SECOND_SYM)},
    {SYM("SECOND_MICROSECOND", SECOND_MICROSECOND_SYM)},
    {SYM("SECONDARY", SECONDARY_SYM)},
    {SYM("SECONDARY_ENGINE", SECONDARY_ENGINE_SYM)},
    {SYM("SECONDARY_ENGINE_ATTRIBUTE", SECONDARY_ENGINE_ATTRIBUTE_SYM)},
    {SYM("SECONDARY_LOAD", SECONDARY_LOAD_SYM)},
    {SYM("SECONDARY_UNLOAD", SECONDARY_UNLOAD_SYM)},
    {SYM("SECURITY", SECURITY_SYM)},
    {SYM("SENSITIVE", SENSITIVE_SYM)},
    {SYM("SEPARATOR", SEPARATOR_SYM)},
    {SYM("SERIAL", SERIAL_SYM)},
    {SYM("SERIALIZABLE", SERIALIZABLE_SYM)},
    {SYM("SESSION", SESSION_SYM)},
    {SYM_PERCONA("SEQUENCE_TABLE", SEQUENCE_TABLE_SYM)},
    {SYM("SERVER", SERVER_SYM)},
    {SYM("SET", SET_SYM)},
    {SYM("SHARE", SHARE_SYM)},
    {SYM("SHOW", SHOW)},
    {SYM("SHUTDOWN", SHUTDOWN)},
    {SYM("SIGNAL", SIGNAL_SYM)},
    {SYM("SIGNED", SIGNED_SYM)},
    {SYM("SIMPLE", SIMPLE_SYM)},
    {SYM("SKIP", SKIP_SYM)},
    {SYM("SLAVE", SLAVE)},
    {SYM("SLOW", SLOW)},
    {SYM("SNAPSHOT", SNAPSHOT_SYM)},
    {SYM("SMALLINT", SMALLINT_SYM)},
    {SYM("SOCKET", SOCKET_SYM)},
    {SYM("SOME", ANY_SYM)},
    {SYM("SONAME", SONAME_SYM)},
    {SYM("SOUNDS", SOUNDS_SYM)},
    {SYM("SOURCE", SOURCE_SYM)},
    {SYM("SOURCE_AUTO_POSITION", SOURCE_AUTO_POSITION_SYM)},
    {SYM("SOURCE_BIND", SOURCE_BIND_SYM)},
    {SYM("SOURCE_COMPRESSION_ALGORITHMS", SOURCE_COMPRESSION_ALGORITHM_SYM)},
    {SYM("SOURCE_CONNECT_RETRY", SOURCE_CONNECT_RETRY_SYM)},
    {SYM("SOURCE_CONNECTION_AUTO_FAILOVER",
         SOURCE_CONNECTION_AUTO_FAILOVER_SYM)},
    {SYM("SOURCE_DELAY", SOURCE_DELAY_SYM)},
    {SYM("SOURCE_HEARTBEAT_PERIOD", SOURCE_HEARTBEAT_PERIOD_SYM)},
    {SYM("SOURCE_HOST", SOURCE_HOST_SYM)},
    {SYM("SOURCE_LOG_FILE", SOURCE_LOG_FILE_SYM)},
    {SYM("SOURCE_LOG_POS", SOURCE_LOG_POS_SYM)},
    {SYM("SOURCE_PASSWORD", SOURCE_PASSWORD_SYM)},
    {SYM("SOURCE_PORT", SOURCE_PORT_SYM)},
    {SYM("SOURCE_PUBLIC_KEY_PATH", SOURCE_PUBLIC_KEY_PATH_SYM)},
    {SYM("SOURCE_RETRY_COUNT", SOURCE_RETRY_COUNT_SYM)},
    {SYM("SOURCE_SSL_CAPATH", SOURCE_SSL_CAPATH_SYM)},
    {SYM("SOURCE_SSL_CA", SOURCE_SSL_CA_SYM)},
    {SYM("SOURCE_SSL_CERT", SOURCE_SSL_CERT_SYM)},
    {SYM("SOURCE_SSL_CIPHER", SOURCE_SSL_CIPHER_SYM)},
    {SYM("SOURCE_SSL_CRL", SOURCE_SSL_CRL_SYM)},
    {SYM("SOURCE_SSL_CRLPATH", SOURCE_SSL_CRLPATH_SYM)},
    {SYM("SOURCE_SSL_KEY", SOURCE_SSL_KEY_SYM)},
    {SYM("SOURCE_SSL", SOURCE_SSL_SYM)},
    {SYM("SOURCE_SSL_VERIFY_SERVER_CERT", SOURCE_SSL_VERIFY_SERVER_CERT_SYM)},
    {SYM("SOURCE_TLS_CIPHERSUITES", SOURCE_TLS_CIPHERSUITES_SYM)},
    {SYM("SOURCE_TLS_VERSION", SOURCE_TLS_VERSION_SYM)},
    {SYM("SOURCE_USER", SOURCE_USER_SYM)},
    {SYM("SOURCE_ZSTD_COMPRESSION_LEVEL", SOURCE_ZSTD_COMPRESSION_LEVEL_SYM)},
    {SYM("SPATIAL", SPATIAL_SYM)},
    {SYM("SPECIFIC", SPECIFIC_SYM)},
    {SYM("SQL", SQL_SYM)},
    {SYM("SQLEXCEPTION", SQLEXCEPTION_SYM)},
    {SYM("SQLSTATE", SQLSTATE_SYM)},
    {SYM("SQLWARNING", SQLWARNING_SYM)},
    {SYM("SQL_AFTER_GTIDS", SQL_AFTER_GTIDS)},
    {SYM("SQL_AFTER_MTS_GAPS", SQL_AFTER_MTS_GAPS)},
    {SYM("SQL_BEFORE_GTIDS", SQL_BEFORE_GTIDS)},
    {SYM("SQL_BIG_RESULT", SQL_BIG_RESULT)},
    {SYM("SQL_BUFFER_RESULT", SQL_BUFFER_RESULT)},
    {SYM("SQL_CALC_FOUND_ROWS", SQL_CALC_FOUND_ROWS)},
    {SYM("SQL_NO_CACHE", SQL_NO_CACHE_SYM)},
    {SYM("SQL_SMALL_RESULT", SQL_SMALL_RESULT)},
    {SYM("SQL_THREAD", SQL_THREAD)},
    {SYM("SQL_TSI_SECOND", SECOND_SYM)},
    {SYM("SQL_TSI_MINUTE", MINUTE_SYM)},
    {SYM("SQL_TSI_HOUR", HOUR_SYM)},
    {SYM("SQL_TSI_DAY", DAY_SYM)},
    {SYM("SQL_TSI_WEEK", WEEK_SYM)},
    {SYM("SQL_TSI_MONTH", MONTH_SYM)},
    {SYM("SQL_TSI_QUARTER", QUARTER_SYM)},
    {SYM("SQL_TSI_YEAR", YEAR_SYM)},
    {SYM("SRID", SRID_SYM)},
    {SYM("SSL", SSL_SYM)},
    {SYM("STACKED", STACKED_SYM)},
    {SYM("START", START_SYM)},
    {SYM("STARTING", STARTING)},
    {SYM("STARTS", STARTS_SYM)},
    {SYM("STATS_AUTO_RECALC", STATS_AUTO_RECALC_SYM)},
    {SYM("STATS_PERSISTENT", STATS_PERSISTENT_SYM)},
    {SYM("STATS_SAMPLE_PAGES", STATS_SAMPLE_PAGES_SYM)},
    {SYM("STATUS", STATUS_SYM)},
    {SYM("STOP", STOP_SYM)},
    {SYM("STORAGE", STORAGE_SYM)},
    {SYM("STORED", STORED_SYM)},
    {SYM("STRAIGHT_JOIN", STRAIGHT_JOIN)},
    {SYM("STREAM", STREAM_SYM)},
    {SYM("STRING", STRING_SYM)},
    {SYM("SUBCLASS_ORIGIN", SUBCLASS_ORIGIN_SYM)},
    {SYM("SUBJECT", SUBJECT_SYM)},
    {SYM("SUBPARTITION", SUBPARTITION_SYM)},
    {SYM("SUBPARTITIONS", SUBPARTITIONS_SYM)},
    {SYM("SUPER", SUPER_SYM)},
    {SYM("SUSPEND", SUSPEND_SYM)},
    {SYM("SWAPS", SWAPS_SYM)},
    {SYM("SWITCHES", SWITCHES_SYM)},
    {SYM("SYSTEM", SYSTEM_SYM)},
    {SYM("TABLE", TABLE_SYM)},
    {SYM("TABLE_NAME", TABLE_NAME_SYM)},
    {SYM("TABLES", TABLES)},
    {SYM("TABLESAMPLE", TABLESAMPLE_SYM)},
    {SYM("TABLESPACE", TABLESPACE_SYM)},
    {SYM("TABLE_CHECKSUM", TABLE_CHECKSUM_SYM)},
    {SYM_PERCONA("TABLE_STATISTICS", TABLE_STATS_SYM)},
    {SYM("TEMPORARY", TEMPORARY)},
    {SYM("TEMPTABLE", TEMPTABLE_SYM)},
    {SYM("TERMINATED", TERMINATED)},
    {SYM("TEXT", TEXT_SYM)},
    {SYM("THAN", THAN_SYM)},
    {SYM("THEN", THEN_SYM)},
    {SYM("THREAD_PRIORITY", THREAD_PRIORITY_SYM)},
    {SYM_PERCONA("THREAD_STATISTICS", THREAD_STATS_SYM)},
    {SYM("TIES", TIES_SYM)},
    {SYM("TIME", TIME_SYM)},
    {SYM("TIMESTAMP", TIMESTAMP_SYM)},
    {SYM("TIMESTAMPADD", TIMESTAMP_ADD)},
    {SYM("TIMESTAMPDIFF", TIMESTAMP_DIFF)},
    {SYM("TINYBLOB", TINYBLOB_SYM)},
    {SYM("TINYINT", TINYINT_SYM)},
    {SYM("TINYTEXT", TINYTEXT_SYN)},
    {SYM("TLS", TLS_SYM)},
    {SYM("TO", TO_SYM)},
    {SYM("TRAILING", TRAILING)},
    {SYM("TRANSACTION", TRANSACTION_SYM)},
    {SYM("TRIGGER", TRIGGER_SYM)},
    {SYM("TRIGGERS", TRIGGERS_SYM)},
    {SYM("TRUE", TRUE_SYM)},
    {SYM("TRUNCATE", TRUNCATE_SYM)},
    {SYM("TYPE", TYPE_SYM)},
    {SYM("TYPES", TYPES_SYM)},
    {SYM("UNBOUNDED", UNBOUNDED_SYM)},
    {SYM("UNCOMMITTED", UNCOMMITTED_SYM)},
    {SYM("UNDEFINED", UNDEFINED_SYM)},
    {SYM("UNDO_BUFFER_SIZE", UNDO_BUFFER_SIZE_SYM)},
    {SYM("UNDOFILE", UNDOFILE_SYM)},
    {SYM("UNDO", UNDO_SYM)},
    {SYM("UNICODE", UNICODE_SYM)},
    {SYM("UNION", UNION_SYM)},
    {SYM("UNIQUE", UNIQUE_SYM)},
    {SYM("UNKNOWN", UNKNOWN_SYM)},
    {SYM("UNLOCK", UNLOCK_SYM)},
    {SYM("UNINSTALL", UNINSTALL_SYM)},
    {SYM("UNREGISTER", UNREGISTER_SYM)},
    {SYM("UNSIGNED", UNSIGNED_SYM)},
    {SYM("UNTIL", UNTIL_SYM)},
    {SYM("UPGRADE", UPGRADE_SYM)},
    {SYM("URL", URL_SYM)},
    {SYM("USAGE", USAGE)},
    {SYM("USE", USE_SYM)},
    {SYM("USER", USER)},
    {SYM("USER_RESOURCES", RESOURCES)},
    {SYM_PERCONA("USER_STATISTICS", USER_STATS_SYM)},
    {SYM("USE_FRM", USE_FRM)},
    {SYM("USING", USING)},
    {SYM("UTC_DATE", UTC_DATE_SYM)},
    {SYM("UTC_TIME", UTC_TIME_SYM)},
    {SYM("UTC_TIMESTAMP", UTC_TIMESTAMP_SYM)},
    {SYM("VALIDATION", VALIDATION_SYM)},
    {SYM("VALUE", VALUE_SYM)},
    {SYM("VALUES", VALUES)},
    {SYM("VARBINARY", VARBINARY_SYM)},
    {SYM("VARCHAR", VARCHAR_SYM)},
    {SYM("VARCHARACTER", VARCHAR_SYM)},
    {SYM("VARIABLES", VARIABLES)},
    {SYM("VARYING", VARYING)},
    {SYM("WAIT", WAIT_SYM)},
    {SYM("WARNINGS", WARNINGS)},
    {SYM("WEEK", WEEK_SYM)},
    {SYM("WEIGHT_STRING", WEIGHT_STRING_SYM)},
    {SYM("WHEN", WHEN_SYM)},
    {SYM("WHERE", WHERE)},
    {SYM("WHILE", WHILE_SYM)},
    {SYM("WINDOW", WINDOW_SYM)},
    {SYM("VCPU", VCPU_SYM)},
    {SYM("VIEW", VIEW_SYM)},
    {SYM("VIRTUAL", VIRTUAL_SYM)},
    {SYM("VISIBLE", VISIBLE_SYM)},
    {SYM("WITH", WITH)},
    {SYM("WITHOUT", WITHOUT_SYM)},
    {SYM("WORK", WORK_SYM)},
    {SYM("WRAPPER", WRAPPER_SYM)},
    {SYM("WRITE", WRITE_SYM)},
    {SYM("X509", X509_SYM)},
    {SYM("XOR", XOR)},
    {SYM("XA", XA_SYM)},
    {SYM("XID", XID_SYM)},
    {SYM("XML", XML_SYM)}, /* LOAD XML Arnold/Erik */
    {SYM("YEAR", YEAR_SYM)},
    {SYM("YEAR_MONTH", YEAR_MONTH_SYM)},
    {SYM("ZEROFILL", ZEROFILL_SYM)},
    {SYM("ZONE", ZONE_SYM)},
    {SYM("||", OR_OR_SYM)},
    /*
      Place keywords that accept optimizer hints below this comment.
    */
    {SYM_HK("DELETE", DELETE_SYM)},
    {SYM_HK("INSERT", INSERT_SYM)},
    {SYM_HK("REPLACE", REPLACE_SYM)},
    {SYM_HK("SELECT", SELECT_SYM)},
    {SYM_HK("UPDATE", UPDATE_SYM)},
    {SYM_HK("OPTIMIZE", OPTIMIZE)},
    {SYM_HK("CALL", CALL_SYM)},
    {SYM_HK("ALTER", ALTER)},
    {SYM_HK("ANALYZE", ANALYZE_SYM)},
    {SYM_HK("CHECK", CHECK_SYM)},
    {SYM_HK("LOAD", LOAD)},
    {SYM_HK("CREATE", CREATE)},
    /*
     Insert new function definitions after that commentary (by alphabetical
     order)
    */
    {SYM_FN("ADDDATE", ADDDATE_SYM)},
    {SYM_FN("BIT_AND", BIT_AND_SYM)},
    {SYM_FN("BIT_OR", BIT_OR_SYM)},
    {SYM_FN("BIT_XOR", BIT_XOR_SYM)},
    {SYM_FN("CAST", CAST_SYM)},
    {SYM_FN("COUNT", COUNT_SYM)},
    {SYM_FN("CURDATE", CURDATE)},
    {SYM_FN("CURTIME", CURTIME)},
    {SYM_FN("DATE_ADD", DATE_ADD_INTERVAL)},
    {SYM_FN("DATE_SUB", DATE_SUB_INTERVAL)},
    {SYM_FN("EXTRACT", EXTRACT_SYM)},
    {SYM_FN("GROUP_CONCAT", GROUP_CONCAT_SYM)},
    {SYM_FN("JSON_OBJECTAGG", JSON_OBJECTAGG)},
    {SYM_FN("JSON_ARRAYAGG", JSON_ARRAYAGG)},
    {SYM_FN("MAX", MAX_SYM)},
    {SYM_FN("MID", SUBSTRING)}, /* unireg function */
    {SYM_FN("MIN", MIN_SYM)},
    {SYM_FN("NOW", NOW_SYM)},
    {SYM_FN("POSITION", POSITION_SYM)},
    {SYM_FN("SESSION_USER", USER)},
    {SYM_FN("STD", STD_SYM)},
    {SYM_FN("STDDEV", STD_SYM)},
    {SYM_FN("STDDEV_POP", STD_SYM)},
    {SYM_FN("STDDEV_SAMP", STDDEV_SAMP_SYM)},
    {SYM_FN("ST_COLLECT", ST_COLLECT_SYM)},
    {SYM_FN("SUBDATE", SUBDATE_SYM)},
    {SYM_FN("SUBSTR", SUBSTRING)},
    {SYM_FN("SUBSTRING", SUBSTRING)},
    {SYM_FN("SUM", SUM_SYM)},
    {SYM_FN("SYSDATE", SYSDATE)},
    {SYM_FN("SYSTEM_USER", USER)},
    {SYM_FN("TRIM", TRIM)},
    {SYM_FN("VARIANCE", VARIANCE_SYM)},
    {SYM_FN("VAR_POP", VARIANCE_SYM)},
    {SYM_FN("VAR_SAMP", VAR_SAMP_SYM)},
    /*
      Insert new optimizer hint keywords after that commentary:
    */
    {SYM_H("BKA", BKA_HINT)},
    {SYM_H("BNL", BNL_HINT)},
    {SYM_H("DUPSWEEDOUT", DUPSWEEDOUT_HINT)},
    {SYM_H("FIRSTMATCH", FIRSTMATCH_HINT)},
    {SYM_H("INTOEXISTS", INTOEXISTS_HINT)},
    {SYM_H("LOOSESCAN", LOOSESCAN_HINT)},
    {SYM_H("MATERIALIZATION", MATERIALIZATION_HINT)},
    {SYM_H("MAX_EXECUTION_TIME", MAX_EXECUTION_TIME_HINT)},
    {SYM_H("NO_BKA", NO_BKA_HINT)},
    {SYM_H("NO_BNL", NO_BNL_HINT)},
    {SYM_H("NO_ICP", NO_ICP_HINT)},
    {SYM_H("NO_MRR", NO_MRR_HINT)},
    {SYM_H("NO_RANGE_OPTIMIZATION", NO_RANGE_OPTIMIZATION_HINT)},
    {SYM_H("NO_SEMIJOIN", NO_SEMIJOIN_HINT)},
    {SYM_H("MRR", MRR_HINT)},
    {SYM_H("QB_NAME", QB_NAME_HINT)},
    {SYM_H("SEMIJOIN", SEMIJOIN_HINT)},
    {SYM_H("SET_VAR", SET_VAR_HINT)},
    {SYM_H("SUBQUERY", SUBQUERY_HINT)},
    {SYM_H("MERGE", DERIVED_MERGE_HINT)},
    {SYM_H("NO_MERGE", NO_DERIVED_MERGE_HINT)},
    {SYM_H("JOIN_PREFIX", JOIN_PREFIX_HINT)},
    {SYM_H("JOIN_SUFFIX", JOIN_SUFFIX_HINT)},
    {SYM_H("JOIN_ORDER", JOIN_ORDER_HINT)},
    {SYM_H("JOIN_FIXED_ORDER", JOIN_FIXED_ORDER_HINT)},
    {SYM_H("INDEX_MERGE", INDEX_MERGE_HINT)},
    {SYM_H("NO_INDEX_MERGE", NO_INDEX_MERGE_HINT)},
    {SYM_H("RESOURCE_GROUP", RESOURCE_GROUP_HINT)},
    {SYM_H("SKIP_SCAN", SKIP_SCAN_HINT)},
    {SYM_H("NO_SKIP_SCAN", NO_SKIP_SCAN_HINT)},
    {SYM_H("HASH_JOIN", HASH_JOIN_HINT)},
    {SYM_H("NO_HASH_JOIN", NO_HASH_JOIN_HINT)},
    {SYM_H("INDEX", INDEX_HINT)},
    {SYM_H("NO_INDEX", NO_INDEX_HINT)},
    {SYM_H("JOIN_INDEX", JOIN_INDEX_HINT)},
    {SYM_H("NO_JOIN_INDEX", NO_JOIN_INDEX_HINT)},
    {SYM_H("GROUP_INDEX", GROUP_INDEX_HINT)},
    {SYM_H("NO_GROUP_INDEX", NO_GROUP_INDEX_HINT)},
    {SYM_H("ORDER_INDEX", ORDER_INDEX_HINT)},
    {SYM_H("NO_ORDER_INDEX", NO_ORDER_INDEX_HINT)},
    {SYM_H("DERIVED_CONDITION_PUSHDOWN", DERIVED_CONDITION_PUSHDOWN_HINT)},
    {SYM_H("NO_DERIVED_CONDITION_PUSHDOWN",
           NO_DERIVED_CONDITION_PUSHDOWN_HINT)}};

#endif /* LEX_INCLUDED */
