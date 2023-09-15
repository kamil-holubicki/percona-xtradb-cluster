/* Copyright (c) 2013, 2023, Oracle and/or its affiliates.

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

#ifndef PARSE_TREE_NODE_BASE_INCLUDED
#define PARSE_TREE_NODE_BASE_INCLUDED

#include <assert.h>
#include <cstdarg>
#include <cstdlib>
#include <new>
#include <queue>
#include <typeinfo>

#include "memory_debugging.h"
#include "my_alloc.h"
#include "my_compiler.h"

#include "mem_root_deque.h"
#include "my_inttypes.h"  // TODO: replace with cstdint
#include "sql-common/json_dom.h"
#include "sql/check_stack.h"
#include "sql/parse_location.h"
#include "sql/sql_const.h"
#include "sql/sql_list.h"

class Query_block;
class THD;

// uncachable cause
#define UNCACHEABLE_DEPENDENT 1
#define UNCACHEABLE_RAND 2
#define UNCACHEABLE_SIDEEFFECT 4
/* For uncorrelated SELECT in an UNION with some correlated SELECTs */
#define UNCACHEABLE_UNITED 8
#define UNCACHEABLE_CHECKOPTION 16

/**
  Names for different query parse tree parts
*/

enum enum_parsing_context {
  CTX_NONE = 0,       ///< Empty value
  CTX_MESSAGE,        ///< "No tables used" messages etc.
  CTX_TABLE,          ///< for single-table UPDATE/DELETE/INSERT/REPLACE
  CTX_SELECT_LIST,    ///< SELECT (subquery), (subquery)...
  CTX_UPDATE_VALUE,   ///< UPDATE ... SET field=(subquery)...
  CTX_INSERT_VALUES,  ///< INSERT ... VALUES
  CTX_INSERT_UPDATE,  ///< INSERT ... ON DUPLICATE KEY UPDATE ...
  CTX_JOIN,
  CTX_QEP_TAB,
  CTX_MATERIALIZATION,
  CTX_DUPLICATES_WEEDOUT,
  CTX_DERIVED,                  ///< "Derived" subquery
  CTX_WHERE,                    ///< Subquery in WHERE clause item tree
  CTX_ON,                       ///< ON clause context
  CTX_WINDOW,                   ///< Named or unnamed window
  CTX_HAVING,                   ///< Subquery in HAVING clause item tree
  CTX_ORDER_BY,                 ///< ORDER BY clause execution context
  CTX_GROUP_BY,                 ///< GROUP BY clause execution context
  CTX_SIMPLE_ORDER_BY,          ///< ORDER BY clause execution context
  CTX_SIMPLE_GROUP_BY,          ///< GROUP BY clause execution context
  CTX_DISTINCT,                 ///< DISTINCT clause execution context
  CTX_SIMPLE_DISTINCT,          ///< DISTINCT clause execution context
  CTX_BUFFER_RESULT,            ///< see SQL_BUFFER_RESULT in the manual
  CTX_ORDER_BY_SQ,              ///< Subquery in ORDER BY clause item tree
  CTX_GROUP_BY_SQ,              ///< Subquery in GROUP BY clause item tree
  CTX_OPTIMIZED_AWAY_SUBQUERY,  ///< Subquery executed once during optimization
  CTX_UNION,
  CTX_UNION_RESULT,  ///< Pseudo-table context for UNION result
  CTX_INTERSECT,
  CTX_INTERSECT_RESULT,  ///< Pseudo-table context
  CTX_EXCEPT,
  CTX_EXCEPT_RESULT,  ///< Pseudo-table context
  CTX_UNARY,
  CTX_UNARY_RESULT,  ///< Pseudo-table context
  CTX_QUERY_SPEC     ///< Inner SELECTs of UNION expression
};

class Query_term;
enum Surrounding_context {
  SC_TOP,
  SC_QUERY_SPECIFICATION,
  SC_TABLE_VALUE_CONSTRUCTOR,
  SC_QUERY_EXPRESSION,
  SC_SUBQUERY,
  SC_UNION_DISTINCT,
  SC_UNION_ALL,
  SC_INTERSECT_DISTINCT,
  SC_INTERSECT_ALL,
  SC_EXCEPT_DISTINCT,
  SC_EXCEPT_ALL
};

struct QueryLevel {
  Surrounding_context m_type;
  mem_root_deque<Query_term *> m_elts;
  bool m_has_order{false};
  QueryLevel(MEM_ROOT *mem_root, Surrounding_context sc, bool has_order = false)
      : m_type(sc), m_elts(mem_root), m_has_order(has_order) {}
};

class Json_object;
class Parse_tree_root;

/**
  Holds the json parse tree being generated by the SHOW PARSE_TREE command.

  The json tree gets built with the help of the two parse tree node walkers
  namely contextualize() and itemize(). Each contextualize() (or itemize())
  creates a new json subtree.  In the beginning of contextualize(),
  push_level() is called, where a new Json object is created and pushed to
  m_json_obj_stack. This object now becomes a parent json object to which all
  the subtrees created by the inner contextualize() calls will be added as
  its children. Each contextualize() will end with a call to pop_level() where
  the current parent will be popped out and made the child of the now-current
  parent object in the stack. This way the json tree is built identical to the
  tree of Parse_tree_node's.
*/
class Show_parse_tree {
 private:
  std::vector<Json_object *> m_json_obj_stack{};
  Json_object_ptr m_root_obj = nullptr;
  Show_parse_tree *m_parent_show_parse_tree = nullptr;

  // Reference position for calculating relative positions of objects in the
  // grammar.
  const char *m_reference_pos = nullptr;

  // Comparator to sort Json_array using 'startpos' field. The children needs
  // to be sorted in the order they occur in the syntax, in order to help
  // generate the original SQL back from the json tree. The order of the
  // contextualize() calls is not guaranteed to be in the syntax order of the
  // objects whose contextualize() functions are called. Hence, the sorting.
  struct Parse_tree_comparator {
    bool operator()(const Json_dom_ptr &a, const Json_dom_ptr &b) const {
      const Json_object *obj_a = down_cast<const Json_object *>(a.get());
      const Json_object *obj_b = down_cast<const Json_object *>(b.get());

      longlong inta = down_cast<Json_int *>(obj_a->get("startpos"))->value();
      longlong intb = down_cast<Json_int *>(obj_b->get("startpos"))->value();
      return inta < intb;
    }
  };
  static constexpr Parse_tree_comparator m_comparator = {};

  /**
    Make the obj a child of the current parent object in the object stack.

    @param      obj     Input json object resembling a parse tree node.

    @retval     false: success; true: error
  */
  bool make_child(Json_object *obj);

  /**
    Pop the current object in the object stack, and return it.
  */
  Json_object *pop_json_object();

 public:
  explicit Show_parse_tree(Show_parse_tree *parent_tree = nullptr)
      : m_parent_show_parse_tree(parent_tree) {}

  /**
    Create a json node out of the parse tree node position and type name, and
    push it onto the json object stack. Effectively, a new json stack level is
    created, to which json children would be added.

    @param      pos     syntax position of the parse tree node
    @param      typname type name of the class of the parse tree node

    @retval     false   success
    @retval     true    error
  */
  bool push_level(const POS &pos, const char *typname);

  /**
    Get the current object under which new json objects are to be added as
    children. Effectively, this is the outermost object in the object stack.

    retval      nullptr if the object stack is empty.
  */
  Json_object *get_current_parent() {
    return m_json_obj_stack.empty()
               ? nullptr
               : static_cast<Json_object *>(m_json_obj_stack.back());
  }

  /**
    Generate the json tree text from the json tree, and return it.

    @retval     non-empty   success
    @retval     empty       error
  */
  std::string get_parse_tree();

  /**
    Pop out the json node pushed by push_level() and make it a child of the
    inner json object in the stack.

    @retval false: success; true: error
  */
  bool pop_level() { return make_child(pop_json_object()); }
};

/**
  Base class for parse tree nodes (excluding the Parse_tree_root hierarchy)
*/
template <typename Context>
class Parse_tree_node_tmpl {
  friend class Item;  // for direct access to the "contextualized" field

  Parse_tree_node_tmpl(const Parse_tree_node_tmpl &);  // undefined
  void operator=(const Parse_tree_node_tmpl &);        // undefined

#ifndef NDEBUG
 private:
  bool contextualized = false;  // true if the node object is contextualized
#endif                          // NDEBUG

 public:
  typedef Context context_t;
  POS m_pos;  // Grammar position. is_empty() if node not created in parser.

  static void *operator new(size_t size, MEM_ROOT *mem_root,
                            const std::nothrow_t &arg
                            [[maybe_unused]] = std::nothrow) noexcept {
    return mem_root->Alloc(size);
  }
  static void operator delete(void *ptr [[maybe_unused]],
                              size_t size [[maybe_unused]]) {
    TRASH(ptr, size);
  }
  static void operator delete(void *, MEM_ROOT *,
                              const std::nothrow_t &) noexcept {}

 protected:
  Parse_tree_node_tmpl() = delete;

  explicit Parse_tree_node_tmpl(const POS &pos) : m_pos(pos) {}

  explicit Parse_tree_node_tmpl(const POS &start_pos, const POS &end_pos) {
    // Range of this item list should be from first character position of
    // start_pos to the last character position of end_pos.
    m_pos.cpp.start = start_pos.cpp.start;
    m_pos.cpp.end = end_pos.cpp.end;
    m_pos.raw.start = start_pos.raw.start;
    m_pos.raw.end = end_pos.raw.end;
  }

  bool begin_parse_tree(Show_parse_tree *tree);
  bool end_parse_tree(Show_parse_tree *tree);

  /**
    Do all context-sensitive things and mark the node as contextualized

    @param      pc      current parse context

    @retval     false   success
    @retval     true    syntax/OOM/etc error
  */
  virtual bool do_contextualize(Context *pc) {
    uchar dummy;
    if (check_stack_overrun(pc->thd, STACK_MIN_SIZE, &dummy)) return true;

#ifndef NDEBUG
    assert(!contextualized);
    contextualized = true;
#endif  // NDEBUG

    return false;
  }

  /**
    Add all the node-specific json fields. Any class that needs to add such
    info should override this function rather than doing it in
    do_contextualize(). E.g. the parse tree node for AVG() may have "distinct"
    field to indicate if AVG(DISTINCT ...) is used or not.

    @param      json_obj      Json object for this parse tree node.
  */
  virtual void add_json_info(Json_object *json_obj [[maybe_unused]]) {}

 public:
  virtual ~Parse_tree_node_tmpl() = default;

#ifndef NDEBUG
  bool is_contextualized() const { return contextualized; }
#endif  // NDEBUG

  // Derived classes should not override this. If needed, they should override
  // do_contextualize().
  // Visual Studio with MSVC_CPPCHECK=ON gives warning C26435:
  // Function <fun> should specify exactly one of
  //    'virtual', 'override', or 'final'
  MY_COMPILER_DIAGNOSTIC_PUSH()
  MY_COMPILER_MSVC_DIAGNOSTIC_IGNORE(26435)
  virtual bool contextualize(Context *pc) final {
    // For condition#2 below ... If position is empty, this item was not
    // created in the parser; so don't show it in the parse tree.
    if (pc->m_show_parse_tree == nullptr || this->m_pos.is_empty())
      return do_contextualize(pc);

    Show_parse_tree *tree = pc->m_show_parse_tree.get();

    if (begin_parse_tree(tree)) return true;

    if (do_contextualize(pc)) return true;

    if (end_parse_tree(tree)) return true;

    return false;
  }
  MY_COMPILER_DIAGNOSTIC_POP()

  /**
    syntax_error() function replacement for deferred reporting of syntax
    errors

    @param      pc      Current parse context.
    @param      pos     Location of the error in lexical scanner buffers.
  */
  void error(Context *pc, const POS &pos) const {
    pc->thd->syntax_error_at(pos);
  }

  /**
    syntax_error() function replacement for deferred reporting of syntax
    errors

    @param      pc      Current parse context.
    @param      pos     Location of the error in lexical scanner buffers.
    @param      msg     Error message.
  */
  void error(Context *pc, const POS &pos, const char *msg) const {
    pc->thd->syntax_error_at(pos, "%s", msg);
  }

  /**
    syntax_error() function replacement for deferred reporting of syntax
    errors

    @param      pc      Current parse context.
    @param      pos     Location of the error in lexical scanner buffers.
    @param      format  Error message format string with optional argument list.
  */
  void errorf(Context *pc, const POS &pos, const char *format, ...) const
      MY_ATTRIBUTE((format(printf, 4, 5)));
};

template <typename Context>
inline void Parse_tree_node_tmpl<Context>::errorf(Context *pc, const POS &pos,
                                                  const char *format,
                                                  ...) const {
  va_list args;
  va_start(args, format);
  pc->thd->vsyntax_error_at(pos, format, args);
  va_end(args);
}

template <typename Context>
bool Parse_tree_node_tmpl<Context>::begin_parse_tree(Show_parse_tree *tree) {
  if (tree == nullptr) return false;
  if (tree->push_level(this->m_pos, typeid(*this).name())) return true;

  // Add node-specific fields. Do it here rather than in end_parse_tree() : We
  // want to show field values *before* they get changed in contextualization.
  // E.g. join type can change from left to right join.
  Json_object *json_obj = tree->get_current_parent();
  assert(json_obj != nullptr);
  add_json_info(json_obj);

  return false;
}

template <typename Context>
bool Parse_tree_node_tmpl<Context>::end_parse_tree(Show_parse_tree *tree) {
  if (tree == nullptr) return false;
  return tree->pop_level();
}

struct Parse_context_base {
  std::unique_ptr<Show_parse_tree> m_show_parse_tree = nullptr;

  explicit Parse_context_base(
      bool show_parse_tree = false,
      Show_parse_tree *parent_show_parent_tree = nullptr) {
    if (show_parse_tree) {
      m_show_parse_tree = std::unique_ptr<Show_parse_tree>(
          new (std::nothrow) Show_parse_tree(parent_show_parent_tree));
    } else
      m_show_parse_tree = nullptr;
  }
};

/**
  Environment data for the contextualization phase
*/
struct Parse_context : public Parse_context_base {
  THD *const thd;                      ///< Current thread handler
  MEM_ROOT *mem_root;                  ///< Current MEM_ROOT
  Query_block *select;                 ///< Current Query_block object
  mem_root_deque<QueryLevel> m_stack;  ///< Aids query term tree construction
  /// Call upon parse completion.
  /// @returns true on error, else false
  bool finalize_query_expression();
  Parse_context(THD *thd, Query_block *sl, bool show_parse_tree,
                Show_parse_tree *parent_show_parent_tree);
  Parse_context(THD *thd_arg, Query_block *sl_arg, bool show_parse_tree = false)
      : Parse_context(thd_arg, sl_arg, show_parse_tree, nullptr) {}
  Parse_context(THD *thd_arg, Query_block *sl_arg,
                Show_parse_tree *parent_show_parent_tree)
      : Parse_context(thd_arg, sl_arg, parent_show_parent_tree != nullptr,
                      parent_show_parent_tree) {}

  bool is_top_level_union_all(
      Surrounding_context op);  ///< Determine if there is anything but
                                ///< UNION ALL above in m_stack
};

typedef Parse_tree_node_tmpl<Parse_context> Parse_tree_node;

#endif /* PARSE_TREE_NODE_BASE_INCLUDED */
