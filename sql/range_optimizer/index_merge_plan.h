/* Copyright (c) 2000, 2021, Oracle and/or its affiliates.

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

#ifndef SQL_RANGE_OPTIMIZER_INDEX_MERGE_PLAN_H_
#define SQL_RANGE_OPTIMIZER_INDEX_MERGE_PLAN_H_

#include "sql/range_optimizer/table_read_plan.h"

class Opt_trace_object;
class RANGE_OPT_PARAM;
class TRP_RANGE;
struct MEM_ROOT;

/*
  Plan for QUICK_INDEX_MERGE_SELECT scan.
  QUICK_ROR_INTERSECT_SELECT always retrieves full rows, so retrieve_full_rows
  is ignored by make_quick.
*/

class TRP_INDEX_MERGE : public TABLE_READ_PLAN {
 public:
  TRP_INDEX_MERGE(TABLE *table_arg, bool forced_by_hint_arg)
      : TABLE_READ_PLAN(table_arg, MAX_KEY, /*used_key_parts=*/0,
                        forced_by_hint_arg) {}
  RowIterator *make_quick(THD *thd, double expected_rows,
                          bool retrieve_full_rows, MEM_ROOT *return_mem_root,
                          ha_rows *examined_rows) override;
  Bounds_checked_array<AccessPath *> range_scans; /* plans of merged scans */

  void trace_basic_info(THD *thd, const RANGE_OPT_PARAM *param,
                        Opt_trace_object *trace_object) const override;

  RangeScanType get_type() const override { return QS_TYPE_INDEX_MERGE; }
  bool is_keys_used(const MY_BITMAP *fields) override;
  void need_sorted_output() override { assert(false); /* Can't do it */ }
  void get_fields_used(MY_BITMAP *used_fields) const override;
  void add_info_string(String *str) const override;
  void add_keys_and_lengths(String *key_names,
                            String *used_lengths) const override;
  unsigned get_max_used_key_length() const final {
    assert(false);
    return 0;
  }
#ifndef NDEBUG
  void dbug_dump(int indent, bool verbose) override;
#endif
};

#endif  // SQL_RANGE_OPTIMIZER_INDEX_MERGE_PLAN_H_
