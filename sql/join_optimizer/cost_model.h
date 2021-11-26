/* Copyright (c) 2020, 2021, Oracle and/or its affiliates.

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

#ifndef SQL_JOIN_OPTIMIZER_COST_MODEL_H_
#define SQL_JOIN_OPTIMIZER_COST_MODEL_H_

#include "my_base.h"
#include "sql/join_optimizer/access_path.h"
#include "sql/join_optimizer/relational_expression.h"
#include "sql/mem_root_array.h"

struct AccessPath;
struct ContainedSubquery;
class Item;
class Query_block;
class THD;
struct TABLE;

// These are extremely arbitrary cost model constants. We should revise them
// based on actual query times (possibly using linear regression?), and then
// put them into the cost model to make them user-tunable.
constexpr double kApplyOneFilterCost = 0.1;
constexpr double kAggregateOneRowCost = 0.1;
constexpr double kSortOneRowCost = 0.1;
constexpr double kHashBuildOneRowCost = 0.1;
constexpr double kHashProbeOneRowCost = 0.1;
constexpr double kHashReturnOneRowCost = 0.07;
constexpr double kMaterializeOneRowCost = 0.1;
constexpr double kWindowOneRowCost = 0.1;

/// See EstimateFilterCost.
struct FilterCost {
  // Cost of evaluating the filter if nothing in particular is done with it.
  double cost_if_not_materialized;

  // Cost of evaluating the filter if all subqueries in it have been
  // materialized beforehand. If there are no subqueries in the condition,
  // equals cost_if_not_materialized.
  double cost_if_materialized;

  // Cost of materializing all subqueries present in the filter.
  // If there are no subqueries in the condition, equals zero.
  double cost_to_materialize;
};

/// Used internally by EstimateFilterCost() only.
void AddCost(THD *thd, const ContainedSubquery &subquery, double num_rows,
             FilterCost *cost);

/**
  Estimate the cost of evaluating “condition”, “num_rows” times.
  This is a fairly rudimentary estimation, _but_ it includes the cost
  of any subqueries that may be present and that need evaluation.
 */
FilterCost EstimateFilterCost(THD *thd, double num_rows, Item *condition,
                              Query_block *outer_query_block);

/**
  A cheaper overload of EstimateFilterCost() that assumes that all
  contained subqueries have already been extracted (ie., it skips the
  walking, which can be fairly expensive). This data is typically
  computed by FindContainedSubqueries().
 */
inline FilterCost EstimateFilterCost(
    THD *thd, double num_rows,
    const Mem_root_array<ContainedSubquery> &contained_subqueries) {
  FilterCost cost{0.0, 0.0, 0.0};
  cost.cost_if_not_materialized = num_rows * kApplyOneFilterCost;
  cost.cost_if_materialized = num_rows * kApplyOneFilterCost;

  for (const ContainedSubquery &subquery : contained_subqueries) {
    AddCost(thd, subquery, num_rows, &cost);
  }
  return cost;
}

double EstimateCostForRefAccess(THD *thd, TABLE *table, unsigned key_idx,
                                double num_output_rows);
void EstimateSortCost(AccessPath *path, ha_rows limit_rows = HA_POS_ERROR);
void EstimateMaterializeCost(THD *thd, AccessPath *path);
void EstimateAggregateCost(AccessPath *path, const Query_block *query_block);
void EstimateDeleteRowsCost(AccessPath *path);

inline double FindOutputRowsForJoin(double left_rows, double right_rows,
                                    const JoinPredicate *edge) {
  double fanout = right_rows * edge->selectivity;
  if (edge->expr->type == RelationalExpression::LEFT_JOIN) {
    // For outer joins, every outer row produces at least one row (if none
    // are matching, we get a NULL-complemented row).
    // Note that this can cause inconsistent row counts; see bug #33550360
    // and/or JoinHypergraph::has_reordered_left_joins.
    fanout = std::max(fanout, 1.0);
  } else if (edge->expr->type == RelationalExpression::SEMIJOIN) {
    // Semi- and antijoin estimation is pretty tricky, since we want isn't
    // really selectivity; we want the probability that at least one row
    // is matching, which is something else entirely. However, given that
    // we only have selectivity to work with, we don't really have anything
    // better than to estimate it as a normal join and cap the result
    // at selectivity 1.0 (ie., each outer row generates at most one inner row).
    fanout = std::min(fanout, 1.0);
  } else if (edge->expr->type == RelationalExpression::ANTIJOIN) {
    // Antijoin are estimated as simply the opposite of semijoin (see above),
    // but wrongly estimating 0 rows (or, of course, a negative amount) could be
    // really bad, so we assume at least 10% coming out as a fudge factor.
    // It's better to estimate too high than too low here.
    fanout = std::max(1.0 - fanout, 0.1);
  }
  return left_rows * fanout;
}

#endif  // SQL_JOIN_OPTIMIZER_COST_MODEL_H_
