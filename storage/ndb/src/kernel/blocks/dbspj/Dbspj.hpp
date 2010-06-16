/* Copyright (C) 2003 MySQL AB

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef DBSPJ_H
#define DBSPJ_H

#include <SimulatedBlock.hpp>
#include <signaldata/LqhKey.hpp>
#include <signaldata/ScanFrag.hpp>
#include <AttributeHeader.hpp>
#include <SLFifoList.hpp>
#include <DLFifoList.hpp>
#include <SLList.hpp>
#include <ArenaPool.hpp>
#include <DataBuffer2.hpp>
#include <signaldata/DbspjErr.hpp>
#include "../dbtup/tuppage.hpp"

class SectionReader;
struct QueryNode;
struct QueryNodeParameters;

class Dbspj: public SimulatedBlock {
public:
  Dbspj(Block_context& ctx, Uint32 instanceNumber = 0);
  virtual ~Dbspj();

private:
  BLOCK_DEFINES(Dbspj);

  /**
   * Signals from TC
   */
  void execLQHKEYREQ(Signal* signal);
  void execSCAN_FRAGREQ(Signal* signal);
  void execSCAN_NEXTREQ(Signal* signal);

  void execDIH_SCAN_TAB_REF(Signal*);
  void execDIH_SCAN_TAB_CONF(Signal*);
  void execDIH_SCAN_GET_NODES_REF(Signal*);
  void execDIH_SCAN_GET_NODES_CONF(Signal*);

  /**
   * Signals from LQH
   */
  void execLQHKEYREF(Signal* signal);
  void execLQHKEYCONF(Signal* signal);
  void execSCAN_FRAGREF(Signal* signal);
  void execSCAN_FRAGCONF(Signal* signal);
  void execSCAN_HBREP(Signal* signal);
  void execTRANSID_AI(Signal*signal);

  /**
   * General signals
   */
  void execDUMP_STATE_ORD(Signal* signal){}
  void execREAD_NODESCONF(Signal*);
  void execREAD_CONFIG_REQ(Signal* signal);
  void execSTTOR(Signal* signal);
  void execDBINFO_SCANREQ(Signal* signal); 
  void execCONTINUEB(Signal*);
  void execNODE_FAILREP(Signal*);
  void execINCL_NODEREQ(Signal*);
  void execAPI_FAILREQ(Signal*);

  void sendSTTORRY(Signal* signal);

protected:
  //virtual bool getParam(const char* name, Uint32* count);

public:
  struct Request;
  struct TreeNode;
  typedef DataBuffer2<14, LocalArenaPoolImpl> Dependency_map;
  typedef LocalDataBuffer2<14, LocalArenaPoolImpl> Local_dependency_map;
  typedef DataBuffer2<14, LocalArenaPoolImpl> PatternStore;
  typedef LocalDataBuffer2<14, LocalArenaPoolImpl> Local_pattern_store;

  struct RowRef
  {
    Uint32 m_page_id;
    Uint16 m_page_pos;
    union 
    {
      Uint16 unused;
      Uint16 m_allocator;
    };

    void copyto_link(Uint32 * dst) const { 
      dst[0] = m_page_id; dst[1] = m_page_pos;
    }
    void assign_from_link(const Uint32 * src) { 
      m_page_id = src[0]; 
      m_page_pos = src[1];
    }

    void copyto_map(Uint16 * dst) const {
      dst[0] = Uint16(m_page_id);
      dst[1] = Uint16(m_page_id >> 16);
      dst[2] = m_page_pos;
    }

    void assign_from_map(const Uint16 * src) {
      m_page_id = src[0];
      m_page_id += Uint32(src[1]) << 16;
      m_page_pos = src[2];
    }

    static bool map_is_null(const Uint16 * src) {
      return src[2] == 0xFFFF;
    }

    void setNull() { m_page_id = RNIL;}
    bool isNull() const { return m_page_id == RNIL;}
  };

  static const RowRef NullRowRef;

  /**
   * This struct represent a row being passed to a child
   */
  struct RowPtr
  {
    Uint32 m_type;
    Uint32 m_src_node_no;
    Uint32 m_src_node_ptrI;
    Uint32 m_src_correlation;

    struct Header
    {
      Uint32 m_len;
      Uint32 m_offset[1];
    };

    struct Section
    {
      const Header * m_header;
      SegmentedSectionPtrPOD m_dataPtr;
    };

    struct Linear
    {
      RowRef m_row_ref;
      const Header * m_header;
      const Uint32 * m_data;
    };
    union
    {
      struct Section m_section;
      struct Linear m_linear;
    } m_row_data;

    enum RowType
    {
      RT_SECTION = 1,
      RT_LINEAR = 2,
      RT_END = 0
    };
  };

  struct SLFifoRowList
  {
    /**
     * Data used for a single linked list of rows
     */
    Uint32 m_first_row_page_id;
    Uint32 m_last_row_page_id;
    Uint16 m_first_row_page_pos;
    Uint16 m_last_row_page_pos;

    void init() { m_first_row_page_id = RNIL;}
    bool isNull() const { return m_first_row_page_id == RNIL; }
  };
  
  struct RowMap
  {
    /**
     * Data used for a map with rows (key is correlation id)
     *   currently a single array is used to store row references
     *   (size == batch size)
     */
    RowRef m_map_ref;
    Uint16 m_size;                // size of array
    Uint16 m_elements;            // #elements in array

    void init() { m_map_ref.setNull();}
    bool isNull() const { return m_map_ref.isNull(); }

    void assign (RowRef ref) {
      m_map_ref = ref;
    }

    void copyto(RowRef& ref) const {
      ref = m_map_ref;
    }

    /**
     * functions for manipulating *content* of map
     */
    void clear(Uint32 * ptr)  { 
      memset(ptr, 0xFF, MAP_SIZE_PER_REF_16 * m_size * sizeof(Uint16));
    }
    void store(Uint32 * _ptr, Uint32 pos, RowRef ref) { 
      Uint16 * ptr = (Uint16*)_ptr;
      ptr += MAP_SIZE_PER_REF_16 * pos;
      ref.copyto_map(ptr);
      m_elements++;
    }
    static void load(const Uint32 * _ptr, Uint32 pos, RowRef & ref) {
      const Uint16 * ptr = (const Uint16*)_ptr;
      ptr += MAP_SIZE_PER_REF_16 * pos;
      ref.assign_from_map(ptr);
    }
    static bool isNull(const Uint32 * _ptr, Uint32 pos) {
      const Uint16 * ptr = (const Uint16*)_ptr;
      ptr += MAP_SIZE_PER_REF_16 * pos;
      return RowRef::map_is_null(ptr);
    }

    STATIC_CONST( MAP_SIZE_PER_REF_16 = 3 );
  };

  struct SLFifoRowListIterator
  {
    RowRef m_ref;
    Uint32 * m_row_ptr;

    bool isNull() const { return m_ref.isNull(); }
    void setNull() { m_ref.setNull(); }
  };

  struct SLFifoRowListIteratorPtr
  {
    RowRef m_ref;
  };

  struct RowMapIterator
  {
    Uint32 * m_row_ptr;
    Uint32 * m_map_ptr;
    RowRef m_ref; // position of actual row
    Uint16 m_size;
    Uint16 m_element_no;
    bool isNull() const { return m_ref.isNull(); }
    void setNull() { m_ref.setNull(); }
  };

  struct RowMapIteratorPtr
  {
    Uint32 m_element_no;
  };


  /**
   * A struct used when building an TreeNode
   */
  struct Build_context
  {
    Uint32 m_cnt;
    Uint32 m_scanPrio;
    Uint32 m_savepointId;
    Uint32 m_batch_size_rows;
    Uint32 m_resultRef;  // API
    Uint32 m_resultData; // API
    Uint32 m_senderRef;  // TC (used for routing)
    Uint32 m_scan_cnt;
    Signal* m_start_signal; // Argument to first node in tree
    SegmentedSectionPtr m_keyPtr;
    Ptr<TreeNode> m_node_list[63];   // Used for resolving dependencies
  };

  struct RowPage
  {
    /**
     * NOTE: This contains various padding to be binary aligned with Tup_page
     *       (for storing into DLFifoList<RowPage>
     */
    struct File_formats::Page_header m_page_header;
    Uint32 unused0;
    Uint32 unused1;
    Uint32 nextList;
    Uint32 prevList;
    Uint32 m_data[GLOBAL_PAGE_SIZE_WORDS - 7];
    STATIC_CONST( SIZE = GLOBAL_PAGE_SIZE_WORDS - 7 );
  };

  typedef Tup_varsize_page Var_page;

  struct RowBuffer 
  {
    RowBuffer() { stack_init(); }
    DLFifoList<RowPage>::Head m_page_list;

    void stack_init() { new (&m_page_list) DLFifoList<RowPage>::Head(); m_stack.m_pos = 0xFFFF; }
    void var_init() { new (&m_page_list) DLFifoList<RowPage>::Head(); m_var.m_free = 0; }

    struct Stack
    {
      Uint32 m_pos; // position on head-page
    };

    struct Var
    {
      Uint32 m_free; // Free on last page in list
    };

    union {
      struct Stack m_stack;
      struct Var m_var;
    };
  };

  /**
   * A struct for building DA-part
   *   that is shared between QN_LookupNode & QN_ScanFragNode
   */
  struct DABuffer
  {
    const Uint32 * ptr;
    const Uint32 * end;
  };

  /**
   * A struct with "virtual" functions for different operations
   */
  struct OpInfo
  {
    /**
     * This function create a operation suitable
     *   for execution
     */
    Uint32 (Dbspj::*m_build)(Build_context&ctx, Ptr<Request>,
                             const QueryNode*, const QueryNodeParameters*);

    /**
     * This function is called after build, but before start
     *   it's allowed to block (i.e send signals)
     *   and should if so increase request::m_outstanding
     */
    void (Dbspj::*m_prepare)(Signal*, Ptr<Request>, Ptr<TreeNode>);

    /**
     * This function is used for starting a request
     */
    void (Dbspj::*m_start)(Signal*, Ptr<Request>, Ptr<TreeNode>);

    /**
     * This function is used when getting a TRANSID_AI
     */
    void (Dbspj::*m_execTRANSID_AI)(Signal*,Ptr<Request>,Ptr<TreeNode>,
				    const RowPtr&);

    /**
     * This function is used when getting a LQHKEYREF
     */
    void (Dbspj::*m_execLQHKEYREF)(Signal*, Ptr<Request>, Ptr<TreeNode>);

    /**
     * This function is used when getting a LQHKEYCONF
     */
    void (Dbspj::*m_execLQHKEYCONF)(Signal*, Ptr<Request>, Ptr<TreeNode>);

    /**
     * This function is used when getting a SCAN_FRAGREF
     */
    void (Dbspj::*m_execSCAN_FRAGREF)(Signal*, Ptr<Request>, Ptr<TreeNode>);

    /**
     * This function is used when getting a SCAN_FRAGCONF
     */
    void (Dbspj::*m_execSCAN_FRAGCONF)(Signal*, Ptr<Request>, Ptr<TreeNode>);

    /**
     * This function is called on the *child* by the *parent* when passing rows
     */
    void (Dbspj::*m_parent_row)(Signal*,Ptr<Request>,Ptr<TreeNode>,
                                const RowPtr&);
    
    /**
     * This function is called on the *child* by the *parent* when *parent*
     *   has completed a batch
     */
    void (Dbspj::*m_parent_batch_complete)(Signal*,Ptr<Request>,Ptr<TreeNode>);

    /**
     * This function is called when getting a SCAN_NEXTREQ
     */
    void (Dbspj::*m_execSCAN_NEXTREQ)(Signal*, Ptr<Request>,Ptr<TreeNode>);

    /**
     * This function is called when all nodes in tree are finished
     *   it's allowed to "block" (by increaseing requestPtr.p->m_outstanding)
     */
    void (Dbspj::*m_complete)(Signal*, Ptr<Request>,Ptr<TreeNode>);

    /**
     * This function is called when a tree is aborted
     *   it's allowed to "block" (by increaseing requestPtr.p->m_outstanding)
     */
    void (Dbspj::*m_abort)(Signal*, Ptr<Request>, Ptr<TreeNode>);

    /**
     * This function is called on node-failure
     */
    Uint32 (Dbspj::*m_execNODE_FAILREP)(Signal*, Ptr<Request>, Ptr<TreeNode>,
                                        NdbNodeBitmask);
    /**
     * This function is called when request/node(s) is/are removed
     *  should only do local cleanup(s)
     */
    void (Dbspj::*m_cleanup)(Ptr<Request>, Ptr<TreeNode>);
  };

  struct LookupData
  {
    Uint32 m_api_resultRef;
    Uint32 m_api_resultData;
    Uint32 m_lqhKeyReq[LqhKeyReq::FixedSignalLength + 4];
  };

  struct ScanFragData
  {
    Uint32 m_rows_received;  // #execTRANSID_AI
    Uint32 m_rows_expecting; // ScanFragConf
    Uint32 m_scanFragReq[ScanFragReq::SignalLength + 2];
  };

  struct ScanIndexFrag
  {
    void init(Uint32 fid) { 
      m_rangePtrI = RNIL; 
      m_ref = 0; 
      m_fragId = fid; 
      m_range_builder.m_range_cnt = 0;
      m_range_builder.m_range_size = 0;
      m_state = Uint16(~0);
    }
    Uint32 m_magic;
    Uint16 m_fragId;
    Uint16 m_state;
    Uint32 m_ref;
    struct RangeBuilder 
    {
      Uint16 m_range_size;
      Uint16 m_range_cnt; // too set bounds info correctly
    } m_range_builder;
    Uint32 m_rangePtrI;
    union {
      Uint32 nextList;
      Uint32 nextPool;
    };
  };

  typedef RecordPool<ScanIndexFrag, ArenaPool> ScanIndexFrag_pool;
  typedef SLFifoListImpl<ScanIndexFrag_pool, ScanIndexFrag> ScanIndexFrag_list;
  typedef LocalSLFifoListImpl<ScanIndexFrag_pool, ScanIndexFrag> Local_ScanIndexFrag_list;

  struct ScanIndexData
  {
    Uint16 m_frags_complete;
    Uint32 m_frags_outstanding;
    Uint32 m_rows_received;  // #execTRANSID_AI
    Uint32 m_rows_expecting; // Sum(ScanFragConf)
    Uint32 m_scanCookie;
    Uint32 m_fragCount;
    Uint32 m_currentFragmentPtrI;
    ScanIndexFrag_list::HeadPOD m_fragments; // ScanFrag states
    union 
    {
      PatternStore::HeadPOD m_prunePattern;
      Uint32 m_constPrunePtrI;
    };
    Uint32 m_scanFragReq[ScanFragReq::SignalLength + 2];
  };

  struct TreeNode_cursor_ptr
  {
    Uint32 nextList;
  };

  /**
   * A node in a Query
   *   (This is an instantiated version of QueryNode in
   *    include/kernel/signal/QueryTree.hpp)
   */
  struct TreeNode : TreeNode_cursor_ptr
  {
    STATIC_CONST ( MAGIC = ~RT_SPJ_TREENODE );

    TreeNode()
    : m_magic(MAGIC), m_state(TN_END),
      m_parentPtrI(RNIL), m_requestPtrI(0)
    {}

    TreeNode(Uint32 request)
    : m_magic(MAGIC),
      m_info(0), m_bits(T_LEAF), m_state(TN_BUILDING),
      m_parentPtrI(RNIL), m_requestPtrI(request),
      nextList(RNIL), prevList(RNIL)
    {
//    m_send.m_ref = 0;
      m_send.m_correlation = 0;
      m_send.m_keyInfoPtrI = RNIL;
      m_send.m_attrInfoPtrI = RNIL;
    };

    const Uint32 m_magic;
    const struct OpInfo* m_info;

    enum TreeNodeState
    {
      /**
       * Initial
       */
      TN_BUILDING = 1,

      /**
       * Tree node is build, but not active
       */
      TN_INACTIVE = 2,

      /**
       * Tree node is active (i.e has outstanding request(s))
       */
      TN_ACTIVE = 3,

      /**
       * Tree node is "finishing" (after TN_INACTIVE)
       */
      TN_COMPLETING = 5,

      /**
       * Tree node is aborting
       */
      TN_ABORTING = 6,

      /**
       * end-marker, not a valid state
       */
      TN_END = 0
    };

    enum TreeNodeBits
    {
      T_ATTR_INTERPRETED = 0x1,

      /**
       * Will node be executed only once (::parent_row())
       *   implies key/attr-info will be disowned (by send-signal)
       */
      T_ONE_SHOT = 0x2,

      /**
       * Is keyinfo "constructed"
       *   (implies key info will be disowned (by send-signal)
       */
      T_KEYINFO_CONSTRUCTED = 0x4,

      /**
       * Is attrinfo "constructed"
       *   (implies attr info will be disowned (by send-signal)
       */
      T_ATTRINFO_CONSTRUCTED = 0x8,

      /**
       * Is this node a leaf-node
       */
      T_LEAF = 0x10,

      /**
       * Does this node have a user projection. (The index access part of
       * an index lookup operation has no user projection, since only the
       * base table tuple is sent to the API.)
       */
      T_USER_PROJECTION = 0x20,

      /**
       * Is this a unique index lookup (on index table)
       *   (implies some extra error handling code)
       */
      T_UNIQUE_INDEX_LOOKUP = 0x40,

      /*
       * Should this node buffers its rows
       */
      T_ROW_BUFFER = 0x80,

      /**
       * Should rows have dictionary (i.e random access capability)
       *  This is typically used when having nodes depending on multiple parents
       *  so that when row gets availble from "last" parent, a key can be
       *  constructed using correlation value from parents
       */
      T_ROW_BUFFER_MAP = 0x100,

      /**
       * Does any child need to know about when *my* batch is complete
       */
      T_REPORT_BATCH_COMPLETE  = 0x200,

      /**
       * Do I need to know when parent batch is cimpleted
       */
      T_NEED_REPORT_BATCH_COMPLETED = 0x400,

      /**
       * Constant prune pattern
       */
      T_CONST_PRUNE = 0x800,

      /**
       * Prune pattern
       */
      T_PRUNE_PATTERN = 0x1000,

      /**
       * Should index scan be parallel
       */
      T_SCAN_PARALLEL = 0x2000,

      // End marker...
      T_END = 0
    };

    bool isLeaf() const { return (m_bits & T_LEAF) != 0;}

    Uint32 m_bits;
    Uint32 m_state;
    Uint32 m_batch_size;
    Uint32 m_parentPtrI;
    const Uint32 m_requestPtrI;
    Dependency_map::Head m_dependent_nodes;
    PatternStore::Head m_keyPattern;
    PatternStore::Head m_attrParamPattern;

    /**
     * Rows buffered by this node
     */
    union
    {
      RowMap m_row_map;
      SLFifoRowList m_row_list;
    };

    union
    {
      LookupData m_lookup_data;
      ScanFragData m_scanfrag_data;
      ScanIndexData m_scanindex_data;
    };

    struct {
      Uint32 m_ref;              // dst for signal
      /** Each tuple has a 16-bit id that is unique within that operation, 
       * batch and SPJ block instance. The upper half word of m_correlation 
       * is the id of the parent tuple, and the lower half word is the 
       * id of the current tuple.*/
      Uint32 m_correlation;
      Uint32 m_keyInfoPtrI;      // keyInfoSection
      Uint32 m_attrInfoPtrI;     // attrInfoSection
    } m_send;

    union {
      Uint32 nextList;
      Uint32 nextPool;
    };
    Uint32 prevList;
  };

  static const Ptr<TreeNode> NullTreeNodePtr;

  typedef RecordPool<TreeNode, ArenaPool> TreeNode_pool;
  typedef DLFifoListImpl<TreeNode_pool, TreeNode> TreeNode_list;
  typedef LocalDLFifoListImpl<TreeNode_pool, TreeNode> Local_TreeNode_list;

  typedef SLListImpl<TreeNode_pool, TreeNode, TreeNode_cursor_ptr>
  TreeNodeCursor_list;
  typedef LocalSLListImpl<TreeNode_pool, TreeNode, TreeNode_cursor_ptr> 
  Local_TreeNodeCursor_list;

  /**
   * A request (i.e a query + parameters)
   */
  struct Request
  {
    enum RequestBits
    {
      RT_SCAN = 0x1            // unbounded result set, scan interface
      ,RT_ROW_BUFFERS = 0x2    // Do any of the node use row-buffering
      ,RT_MULTI_SCAN  = 0x4    // Is there several scans in request
      ,RT_VAR_ALLOC   = 0x8    // Is var-allocation used for row-buffer
      ,RT_NEED_PREPARE = 0x10  // Does any node need m_prepare hook
      ,RT_NEED_COMPLETE = 0x20 // Does any node need m_complete hook
    };

    enum RequestState
    {
      RS_BUILDING   = 0x1,
      RS_PREPARING  = 0x2,
      RS_RUNNING    = 0x3,
      RS_COMPLETING = 0x4,
      RS_ABORTING   = 0x1000, // Or:ed together with other states

      RS_END = 0
    };

    Request() {}
    Request(const ArenaHead & arena) : m_arena(arena) {}
    Uint32 m_magic;
    Uint32 m_bits;
    Uint32 m_state;
    Uint32 m_errCode;
    Uint32 m_node_cnt;
    Uint32 m_senderRef;
    Uint32 m_senderData;
    Uint32 m_rootResultData;
    Uint32 m_transId[2];
    TreeNode_list::Head m_nodes;
    TreeNodeCursor_list::Head m_cursor_nodes;
    Uint32 m_cnt_active;       // No of "running" nodes
    Uint32 m_rows;             // Rows accumulated in current batch
    Uint32 m_outstanding;      // Outstanding signals, when 0, batch is done
    Uint16 m_lookup_node_data[MAX_NDB_NODES];
    ArenaHead m_arena;
    RowBuffer m_rowBuffer;

    bool isScan() const { return (m_bits & RT_SCAN) != 0;}
    bool isLookup() const { return (m_bits & RT_SCAN) == 0;}

    bool equal(const Request & key) const {
      return
	m_senderData == key.m_senderData &&
	m_transId[0] == key.m_transId[0] &&
	m_transId[1] == key.m_transId[1];
    }

    Uint32 hashValue() const {
      return m_transId[0] ^ m_senderData;
    }

    union {
      Uint32 nextHash;
      Uint32 nextPool;
    };
    Uint32 prevHash;
  };

private:
  /**
   * These are the rows in ndbinfo.counters that concerns the SPJ block.
   * @see Ndbinfo::counter_id.
   */
  enum CounterId
  {
    /**
     * This is the number of incomming LQHKEYREQ messages (i.e queries with a
     * lookup as root).
     */
    CI_READS_RECEIVED = 0,

    /**
     * This is the number of lookup operations (LQHKEYREQ) sent to a local
     * LQH block.
     */
    CI_LOCAL_READS_SENT = 1,

    /**
     * This is the number of lookup operations (LQHKEYREQ) sent to a remote
     * LQH block.
     */
    CI_REMOTE_READS_SENT = 2,

    /**
     * This is the number of incomming queries where the root operation is a 
     * fragment scan and this is a "direct scan" that does not go via an index.
     */
    CI_TABLE_SCANS_RECEIVED = 3,

    /**
     * This is the number of "direct" fragment scans (i.e. no via an ordered 
     * index)sent to the local LQH block.
     */
    CI_LOCAL_TABLE_SCANS_SENT = 4,

    /**
     * This is the number of incomming queries where the root operation is a 
     * fragment scan which scans the fragment via an ordered index..
     */
    CI_RANGE_SCANS_RECEIVED = 5,

    /**
     * This the number of scans using ordered indexes that have been sent to the
     * local LQH block.
     */
    CI_LOCAL_RANGE_SCANS_SENT = 6,

    /**
     * This the number of scans using ordered indexes that have been sent to a
     * remote LQH block.
     */
    CI_REMOTE_RANGE_SCANS_SENT = 7,
    
    /**
     * No of prunable indexscans that has been received
     */
    CI_PRUNNED_RANGE_SCANS_RECEIVED = 8,

    /**
     * No of "const" prunable index scans that has been received 
     * i.e index-scan only access 1 partition
     */
    CI_CONST_PRUNNED_RANGE_SCANS_RECEIVED = 9,

    CI_END = 10 // End marker - not a valid counter id. 
  };

  /**
   * This is a set of counters for monitoring the behavior of the SPJ block.
   * They may be read through the ndbinfo.counters SQL table.
   */
  class MonotonicCounters {
  public:

    MonotonicCounters()
    {
      for(int i = 0; i < CI_END; i++)
      {
        m_counters[i] = 0;
      }
    }

    Uint64 get_counter(CounterId id) const
    {
      return m_counters[id];
    }

    void incr_counter(CounterId id, Uint64 delta)
    {
      m_counters[id] += delta;
    }

  private:
    Uint64 m_counters[CI_END];

  } c_Counters;

  typedef RecordPool<Request, ArenaPool> Request_pool;
  typedef DLListImpl<Request_pool, Request> Request_list;
  typedef LocalDLListImpl<Request_pool, Request> Local_Request_list;
  typedef DLHashTableImpl<Request_pool, Request> Request_hash;
  typedef DLHashTableImpl<Request_pool, Request>::Iterator Request_iterator;

  ArenaAllocator m_arenaAllocator;
  Request_pool m_request_pool;
  Request_hash m_scan_request_hash;
  Request_hash m_lookup_request_hash;
  ArenaPool m_dependency_map_pool;
  TreeNode_pool m_treenode_pool;
  ScanIndexFrag_pool m_scanindexfrag_pool;

  NdbNodeBitmask c_alive_nodes;

  void do_init(Request*, const LqhKeyReq*, Uint32 senderRef);
  void store_lookup(Ptr<Request>);
  void handle_early_lqhkey_ref(Signal*, const LqhKeyReq *, Uint32 err);
  void sendTCKEYREF(Signal* signal, Uint32 ref, Uint32 routeRef);
  void sendTCKEYCONF(Signal* signal, Uint32 len, Uint32 ref, Uint32 routeRef);

  void do_init(Request*, const ScanFragReq*, Uint32 senderRef);
  void store_scan(Ptr<Request>);
  void handle_early_scanfrag_ref(Signal*, const ScanFragReq *, Uint32 err);

  struct BuildKeyReq
  {
    Uint32 hashInfo[4]; // Used for hashing
    Uint32 fragId;
    Uint32 fragDistKey;
    Uint32 receiverRef; // NodeId + InstanceNo
  };

  /**
   * Build
   */
  const OpInfo* getOpInfo(Uint32 op);
  Uint32 build(Build_context&,Ptr<Request>,SectionReader&,SectionReader&);
  void checkPrepareComplete(Signal*, Ptr<Request>, Uint32 cnt);
  void start(Signal*, Ptr<Request>);
  void checkBatchComplete(Signal*, Ptr<Request>, Uint32 cnt);
  void batchComplete(Signal*, Ptr<Request>);
  void sendConf(Signal*, Ptr<Request>, bool is_complete);
  void complete(Signal*, Ptr<Request>);
  void cleanup(Ptr<Request>);
  void abort(Signal*, Ptr<Request>, Uint32 errCode);
  Uint32 nodeFail(Signal*, Ptr<Request>, NdbNodeBitmask mask);

  Uint32 createNode(Build_context&, Ptr<Request>, Ptr<TreeNode> &);
  void reportBatchComplete(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void releaseScanBuffers(Ptr<Request> requestPtr);
  void releaseRequestBuffers(Ptr<Request> requestPtr, bool reset);
  void releaseNodeRows(Ptr<Request> requestPtr, Ptr<TreeNode>);
  void releaseRow(Ptr<Request>, RowRef ref);
  Uint32 releaseScanBuffers(Ptr<Request> requestPtr, Ptr<TreeNode>);
  void registerCursor(Ptr<Request>, Ptr<TreeNode>);
  void nodeFail_checkRequests(Signal*);

  void cleanup_common(Ptr<Request>, Ptr<TreeNode>);

  /**
   * Row buffering
   */
  Uint32 storeRow(Ptr<Request>, Ptr<TreeNode>, RowPtr &row);
  Uint32* stackAlloc(RowBuffer& dst, RowRef&, Uint32 len);
  Uint32* varAlloc(RowBuffer& dst, RowRef&, Uint32 len);

  void add_to_list(SLFifoRowList & list, RowRef rowref);
  Uint32 add_to_map(Ptr<Request> requestPtr, Ptr<TreeNode>, Uint32, RowRef);
  Uint32 * get_row_ptr(const RowMap&, RowMapIterator pos);
  void setupRowPtr(Ptr<TreeNode>, RowPtr& dst, RowRef, const Uint32 * src);

  // NOTE: ref contains info about it being stack/var
  // so adding an inline would be nice...but that remove possibility
  // to add jam()'s
  Uint32 * get_row_ptr_stack(RowRef pos);
  Uint32 * get_row_ptr_var(RowRef pos);

  /**
   * SLFifoRowListIterator
   */
  bool first(Ptr<Request>, Ptr<TreeNode>, SLFifoRowListIterator&);
  bool next(SLFifoRowListIterator&);
  bool next(Ptr<Request>, Ptr<TreeNode>, SLFifoRowListIterator&, SLFifoRowListIteratorPtr);

  bool first(Ptr<Request>, Ptr<TreeNode>, RowMapIterator&);
  bool next(RowMapIterator&);
  bool next(Ptr<Request>,Ptr<TreeNode>, RowMapIterator&, RowMapIteratorPtr);

  /**
   * Misc
   */
  Uint32 buildRowHeader(RowPtr::Header *, SegmentedSectionPtr);
  Uint32 buildRowHeader(RowPtr::Header *, const Uint32 *& src, Uint32 len);
  void getCorrelationData(const RowPtr::Section & row, Uint32 col,
                          Uint32& rootStreamId, Uint32& correlationNumber);
  void getCorrelationData(const RowPtr::Linear & row, Uint32 col,
                          Uint32& rootStreamId, Uint32& correlationNumber);
  Uint32 appendToPattern(Local_pattern_store &, DABuffer & tree, Uint32);
  Uint32 appendParamToPattern(Local_pattern_store&,const RowPtr::Linear&,
                              Uint32);

  Uint32 appendTreeToSection(Uint32 & ptrI, SectionReader &, Uint32);
  Uint32 appendColToSection(Uint32 & ptrI, const RowPtr::Linear&, Uint32 col);
  Uint32 appendColToSection(Uint32 & ptrI, const RowPtr::Section&, Uint32 col);
  Uint32 appendPkColToSection(Uint32 & ptrI, const RowPtr::Section&,Uint32 col);
  Uint32 appendPkColToSection(Uint32 & ptrI, const RowPtr::Linear&, Uint32 col);
  Uint32 appendAttrinfoToSection(Uint32 &, const RowPtr::Linear&, Uint32 col);
  Uint32 appendAttrinfoToSection(Uint32 &, const RowPtr::Section&, Uint32 col);
  Uint32 appendDataToSection(Uint32 & ptrI, Local_pattern_store&,
			     Local_pattern_store::ConstDataBufferIterator&,
			     Uint32 len);
  Uint32 appendFromParent(Uint32 & ptrI, Local_pattern_store&,
                          Local_pattern_store::ConstDataBufferIterator&,
                          Uint32 level, const RowPtr&);
  Uint32 expand(Uint32 & ptrI, Local_pattern_store& p, const RowPtr& r){
    switch(r.m_type){
    case RowPtr::RT_SECTION:
      return expandS(ptrI, p, r);
    case RowPtr::RT_LINEAR:
      return expandL(ptrI, p, r);
    }
    return DbspjErr::InternalError;
  }
  Uint32 expandS(Uint32 & ptrI, Local_pattern_store&, const RowPtr&);
  Uint32 expandL(Uint32 & ptrI, Local_pattern_store&, const RowPtr&);
  Uint32 expand(Uint32 & ptrI, DABuffer& pattern, Uint32 len,
                DABuffer & param, Uint32 cnt);
  Uint32 expand(Local_pattern_store& dst, Ptr<TreeNode> treeNodePtr,
                DABuffer & pattern, Uint32 len,
                DABuffer & param, Uint32 cnt);
  Uint32 parseDA(Build_context&, Ptr<Request>, Ptr<TreeNode>,
                 DABuffer tree, Uint32 treeBits,
                 DABuffer param, Uint32 paramBits);

  Uint32 zeroFill(Uint32 & ptrI, Uint32 cnt);
  Uint32 createEmptySection(Uint32 & ptrI);

  Uint32 getResultRef(Ptr<Request> requestPtr);

  /**
   * Lookup
   */
  static const OpInfo g_LookupOpInfo;
  Uint32 lookup_build(Build_context&,Ptr<Request>,
		      const QueryNode*, const QueryNodeParameters*);
  void lookup_start(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void lookup_send(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void lookup_execTRANSID_AI(Signal*, Ptr<Request>, Ptr<TreeNode>,
			     const RowPtr&);
  void lookup_execLQHKEYREF(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void lookup_execLQHKEYCONF(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void lookup_parent_row(Signal*, Ptr<Request>, Ptr<TreeNode>, const RowPtr &);
  void lookup_parent_batch_complete(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void lookup_abort(Signal*, Ptr<Request>, Ptr<TreeNode>);
  Uint32 lookup_execNODE_FAILREP(Signal*signal, Ptr<Request>, Ptr<TreeNode>,
                               NdbNodeBitmask);
  void lookup_cleanup(Ptr<Request>, Ptr<TreeNode>);

  Uint32 handle_special_hash(Uint32 tableId, Uint32 dstHash[4],
                             const Uint64* src,
                             Uint32 srcLen,       // Len in #32bit words
                             const struct KeyDescriptor* desc);

  Uint32 computeHash(Signal*, BuildKeyReq&, Uint32 table, Uint32 keyInfoPtrI);
  Uint32 getNodes(Signal*, BuildKeyReq&, Uint32 tableId);

  /**
   * ScanFrag
   */
  static const OpInfo g_ScanFragOpInfo;
  Uint32 scanFrag_build(Build_context&, Ptr<Request>,
                        const QueryNode*, const QueryNodeParameters*);
  void scanFrag_start(Signal*, Ptr<Request>,Ptr<TreeNode>);
  void scanFrag_send(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanFrag_execTRANSID_AI(Signal*, Ptr<Request>, Ptr<TreeNode>,
			       const RowPtr &);
  void scanFrag_execSCAN_FRAGREF(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanFrag_execSCAN_FRAGCONF(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanFrag_parent_row(Signal*,Ptr<Request>,Ptr<TreeNode>, const RowPtr &);
  void scanFrag_parent_batch_complete(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanFrag_execSCAN_NEXTREQ(Signal*, Ptr<Request>,Ptr<TreeNode>);
  void scanFrag_abort(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanFrag_cleanup(Ptr<Request>, Ptr<TreeNode>);

  /**
   * ScanIndex
   */
  static const OpInfo g_ScanIndexOpInfo;
  Uint32 scanIndex_build(Build_context&, Ptr<Request>,
                         const QueryNode*, const QueryNodeParameters*);
  Uint32 parseScanIndex(Build_context&, Ptr<Request>, Ptr<TreeNode>,
                        DABuffer tree, Uint32 treeBits,
                        DABuffer param, Uint32 paramBits);
  void scanIndex_prepare(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanIndex_execTRANSID_AI(Signal*, Ptr<Request>, Ptr<TreeNode>,
                                const RowPtr &);
  void scanIndex_execSCAN_FRAGREF(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanIndex_execSCAN_FRAGCONF(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanIndex_parent_row(Signal*,Ptr<Request>,Ptr<TreeNode>, const RowPtr&);
  void scanIndex_fixupBound(Ptr<ScanIndexFrag> fragPtr, Uint32 ptrI, Uint32);
  void scanIndex_send(Signal*,Ptr<Request>,Ptr<TreeNode>);
  void scanIndex_batchComplete(Signal* signal);
  Uint32 scanIndex_findFrag(Local_ScanIndexFrag_list &, Ptr<ScanIndexFrag>&,
                            Uint32 fragId);
  void scanIndex_parent_batch_complete(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanIndex_execSCAN_NEXTREQ(Signal*, Ptr<Request>,Ptr<TreeNode>);
  void scanIndex_complete(Signal*, Ptr<Request>, Ptr<TreeNode>);
  void scanIndex_abort(Signal*, Ptr<Request>, Ptr<TreeNode>);
  Uint32 scanIndex_execNODE_FAILREP(Signal*signal, Ptr<Request>, Ptr<TreeNode>,
                                  NdbNodeBitmask);
  void scanIndex_cleanup(Ptr<Request>, Ptr<TreeNode>);

  /**
   * Page manager
   */
  bool allocPage(Ptr<RowPage> &);
  void releasePage(Ptr<RowPage>);
  void releasePages(Uint32 first, Ptr<RowPage> last);
  void releaseGlobal(Signal*);
  SLList<RowPage>::Head m_free_page_list;
  ArrayPool<RowPage> m_page_pool;

  /**
   * Scratch buffers...
   */
  Uint32 m_buffer0[8192]; // 32k
  Uint32 m_buffer1[8192]; // 32k
};

#endif
