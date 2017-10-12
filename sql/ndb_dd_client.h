/*
   Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.

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

#ifndef NDB_DD_CLIENT_H
#define NDB_DD_CLIENT_H

#include <vector>
#include <unordered_set>

#include "my_inttypes.h"
#include "sql/dd/string_type.h"


namespace dd {
  typedef String_type sdi_t;
  namespace cache {
    class Dictionary_client;
  }
}


/*
  Class encapculating the code for accessing the DD
  from ndbcluster

  Handles:
   - locking and releasing MDL(metadata locks)
   - disabling and restoring autocommit
   - transaction commit and rollback, will automatically
     rollback in case commit has not been called
*/

class Ndb_dd_client {
  class THD* const m_thd;
  dd::cache::Dictionary_client* m_client;
  void* m_auto_releaser; // Opaque pointer
  bool m_mdl_locks_acquired;
  ulonglong m_save_option_bits;
  bool m_comitted;

  void disable_autocommit();

public:
  Ndb_dd_client(class THD* thd);

  ~Ndb_dd_client();

  // Metadata lock functions
  bool mdl_lock_schema(const char* schema_name);
  bool mdl_lock_table(const char* schema_name, const char* table_name);
  bool mdl_locks_acquire_exclusive(const char* schema_name,
                                   const char* table_name);
  void mdl_locks_release();

  // Transaction handling functions
  void commit();
  void rollback();

  bool check_table_exists(const char* schema_name, const char* table_name,
                          int& table_id, int& table_version,
                          dd::String_type* engine);
  bool get_engine(const char* schema_name, const char* table_name,
                  dd::String_type* engine);

  bool rename_table(const char *old_schema_name, const char *old_table_name,
                    const char *new_schema_name, const char *new_table_name,
                    int new_table_id, int new_table_version);
  bool drop_table(const char* schema_name, const char* table_name);
  bool install_table(const char* schema_name, const char* table_name,
                     const dd::sdi_t &sdi,
                     int ndb_table_id, int ndb_table_version,
                     bool force_overwrite);

  bool fetch_schema_names(class std::vector<std::string>*);
  bool get_ndb_table_names_in_schema(const char* schema_name,
                                     std::unordered_set<std::string> *names);
  bool have_local_tables_in_schema(const char* schema_name,
                                   bool* found_local_tables);
  bool schema_exists(const char* schema_name, bool* schema_exists);
};




#endif
