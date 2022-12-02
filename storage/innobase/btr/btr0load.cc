/*****************************************************************************

Copyright (c) 2014, 2022, Oracle and/or its affiliates.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2.0, as published by the
Free Software Foundation.

This program is also distributed with certain software (including but not
limited to OpenSSL) that is licensed under separate terms, as designated in a
particular file or component or in included license documentation. The authors
of MySQL hereby grant you an additional permission to link the program and
your derivative works with the separately licensed software that they have
included with MySQL.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License, version 2.0,
for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

*****************************************************************************/

/** @file btr/btr0load.cc
 The B-tree bulk load

 Created 03/11/2014 Shaohua Wang
 *******************************************************/

#ifndef _WIN32
#include <sys/uio.h>
#endif /* _WIN32 */

#include "arch0arch.h"
#include "btr0btr.h"
#include "btr0cur.h"
#include "btr0load.h"
#include "btr0pcur.h"
#include "buf0buddy.h"
#include "ibuf0ibuf.h"
#include "lob0lob.h"
#include "log0chkp.h"
#include "log0log.h"
#include "scope_guard.h"
#include "trx0trx.h"
#include "ut0ut.h"

namespace ddl {
/** Innodb B-tree index fill factor for bulk load. */
long fill_factor;
}  // namespace ddl

#ifdef UNIV_DEBUG
static bool g_slow_io_debug = false;
void bulk_load_enable_slow_io_debug() { g_slow_io_debug = true; }
void bulk_load_disable_slow_io_debug() { g_slow_io_debug = false; }
#endif /* UNIV_DEBUG */

void Bulk_flusher::start() {
  set_max_queue_size();
  std::thread flush_thread([this]() { run(); });
  m_flush_thread = std::move(flush_thread);
}

Bulk_flusher::~Bulk_flusher() {
  if (m_flush_thread.joinable()) {
    wait_to_stop();
  }
}

void Bulk_flusher::wait_to_stop() {
  ut_ad(m_flush_thread.joinable());
  m_stop = true;
  m_flush_thread.join();
}

void Bulk_flusher::do_work() {
  for (auto &page_extent : m_priv_queue) {
#ifdef UNIV_DEBUG
    if (g_slow_io_debug) {
      std::this_thread::sleep_for(std::chrono::milliseconds{2});
    }
#endif /* UNIV_DEBUG */
    m_pages_flushed += page_extent->used_pages();
    page_extent->flush();
    page_extent->destroy();
    Page_extent::drop(page_extent);
  }
  m_priv_queue.clear();
  const size_t queue_size = get_queue_size();
  const size_t max_queue_size = get_max_queue_size();
  if (queue_size < max_queue_size) {
    m_queue_full = false;
  }
}

void Bulk_flusher::set_max_queue_size() {
  /* Based on buffer pool size, adjust the queue size. */
  const size_t buf_pool_size_in_bytes = (size_t)srv_buf_pool_curr_size;
  const size_t buf_pool_size_in_extents =
      buf_pool_size_in_bytes / (FSP_EXTENT_SIZE * UNIV_PAGE_SIZE);
  const size_t max_size = (buf_pool_size_in_extents / 3);
  const size_t max_limit = 5;
  m_max_queue_size = max_size < 2 ? 2 : std::min(max_limit, max_size);
}

void Bulk_flusher::add(Page_extent *page_extent) {
  const size_t max_queue_size = get_max_queue_size();
  m_mutex.lock();
  while (m_queue.size() >= max_queue_size) {
    m_mutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds{10});
    m_mutex.lock();
  }
  m_queue.push_back(page_extent);
  if (m_queue.size() == max_queue_size) {
    m_queue_full = true;
  }
  m_mutex.unlock();
}

size_t Bulk_flusher::get_queue_size() const {
  std::lock_guard<std::mutex> guard(m_mutex);
  return m_queue.size();
}

bool Bulk_flusher::is_work_available() {
  std::lock_guard<std::mutex> guard(m_mutex);
  if (m_queue.empty()) {
    return false;
  }
  std::copy(m_queue.begin(), m_queue.end(), std::back_inserter(m_priv_queue));
  m_queue.clear();
  ut_ad(m_queue.empty());
  return true;
}

void Bulk_flusher::run() {
  m_n_sleep = 0;
  while (!should_i_stop()) {
    if (is_work_available()) {
      do {
        do_work();
      } while (is_work_available());
    } else {
      sleep();
    }
  }
  while (is_work_available()) {
    do_work();
  }
  info();
}

#ifdef UNIV_DEBUG
static void check_page(dict_index_t *index, const page_no_t page_no) {
  const page_id_t page_id(index->space, page_no);
  const page_size_t page_size = dict_table_page_size(index->table);
  buf_page_force_evict(page_id, page_size);
  mtr_t mtr;
  mtr.start();
  mtr.x_lock(dict_index_get_lock(index), UT_LOCATION_HERE);

  auto block = btr_block_get(page_id, page_size, RW_X_LATCH, UT_LOCATION_HERE,
                             index, &mtr);
  const bool check_lsn = true;
  const bool skip_checksum = fsp_is_checksum_disabled(index->space);
  void *zip = block->get_page_zip();
  auto buf =
      (zip != nullptr) ? block->page.zip.data : buf_block_get_frame(block);
  ut_ad(!buf_page_t::is_zeroes(static_cast<byte *>(buf), page_size.physical()));
  BlockReporter reporter =
      BlockReporter(check_lsn, buf, page_size, skip_checksum);
  const bool is_corrupted = reporter.is_corrupted();
  ut_ad(!is_corrupted);

  mtr.commit();
  buf_page_force_evict(page_id, page_size);
}
#endif /* UNIV_DEBUG */

[[nodiscard]] bool Page_load::is_zblob() const {
  bool is_compressed_blob = true;
  const page_type_t page_type = m_block->get_page_type();
  switch (page_type) {
    case FIL_PAGE_TYPE_ZBLOB:
    case FIL_PAGE_TYPE_ZBLOB2:
    case FIL_PAGE_TYPE_ZLOB_FIRST:
    case FIL_PAGE_TYPE_ZLOB_DATA:
    case FIL_PAGE_TYPE_ZLOB_INDEX:
    case FIL_PAGE_TYPE_ZLOB_FRAG:
    case FIL_PAGE_TYPE_ZLOB_FRAG_ENTRY:
      break;
    default:
      is_compressed_blob = false;
      break;
  }
  return is_compressed_blob;
}

bool Page_load::is_corrupted() const {
  const page_size_t page_size = dict_table_page_size(m_index->table);
  const bool skip_checksum = fsp_is_checksum_disabled(m_index->space);
  const bool check_lsn = true;
  void *zip = m_block->get_page_zip();
  auto frame = buf_block_get_frame(m_block);
  auto buf = (zip != nullptr) ? m_block->page.zip.data : frame;
  BlockReporter reporter =
      BlockReporter(check_lsn, buf, page_size, skip_checksum);
  const bool is_corrupted = reporter.is_corrupted();
  ut_ad(!is_corrupted);
  return is_corrupted;
}

void Page_load::init_for_writing() {
  ut_ad(m_block->is_memory());
  ut_ad(m_mtr == nullptr);
  const space_id_t space_id = m_index->space;
  const bool skip_checksum = fsp_is_checksum_disabled(space_id);
  const bool skip_lsn_check = false;
  const lsn_t page_lsn = log_get_lsn(*log_sys);
  auto buf_pool = buf_pool_get(m_block->page.id);

  if (!fsp_is_system_temporary(space_id) && buf_pool->is_tracking()) {
    const bool force = true;
    buf_page_t *bpage = reinterpret_cast<buf_page_t *>(m_block);
    ut_ad(page_lsn >= buf_pool->track_page_lsn);
    arch_page_sys->track_page(bpage, buf_pool->track_page_lsn, page_lsn, force);
  }

  void *zip = m_block->get_page_zip();
  auto frame = buf_block_get_frame(m_block);
  buf_flush_init_for_writing(m_block, frame, zip, page_lsn, skip_checksum,
                             skip_lsn_check);
  ut_ad(!is_corrupted());
}

#ifdef _WIN32
dberr_t Page_extent::bulk_flush_win() { return flush_one_by_one(); }
#elif defined(UNIV_SOLARIS)
/* Nothing here. */
#else
dberr_t Page_extent::bulk_flush_linux() {
  dberr_t err{DB_SUCCESS};
  const page_no_t n_pages = m_page_loads.size();
  struct iovec *iov = static_cast<struct iovec *>(ut::malloc_withkey(
      UT_NEW_THIS_FILE_PSI_KEY, sizeof(struct iovec) * n_pages));
  const size_t page_size = m_page_loads[0]->get_page_size();
  const space_id_t space_id = m_page_loads[0]->space();

  size_t i = 0;
  for (auto &page_load : m_page_loads) {
    page_load->init_for_writing();
    page_zip_des_t *page_zip = page_load->get_page_zip();
    iov[i].iov_base =
        (page_zip == nullptr) ? page_load->get_page() : page_zip->data;
    ut_ad(iov[i].iov_base != nullptr);
    iov[i].iov_len = page_size; /* Physical page size */
    ut_ad(!buf_page_t::is_zeroes(static_cast<byte *>(iov[i].iov_base),
                                 iov[i].iov_len));
    i++;
  }
  fil_node_t *node;
  page_no_t min_page_no = m_range.first;
  err = fil_prepare_file_for_io(space_id, min_page_no, &node);
  ut_ad(err == DB_SUCCESS);
  const os_offset_t offset = min_page_no * page_size;
  const ssize_t req_bytes = n_pages * page_size;
  ut_ad(node->is_open);
  ssize_t n = pwritev(node->handle.m_file, iov, n_pages, offset);
  if (n != req_bytes) {
    char errbuf[MYSYS_STRERROR_SIZE];
    my_error(ER_ERROR_ON_WRITE, MYF(0), node->name, errno,
             my_strerror(errbuf, sizeof(errbuf), errno));
    err = DB_IO_ERROR;
  }
  ut_ad(n == req_bytes);
  fil_complete_write(space_id, node);
  ut::free(iov);
  return err;
}
#endif /* _WIN32 */

dberr_t Page_extent::flush_one_by_one() {
  dberr_t err{DB_SUCCESS};
  const space_id_t space_id = m_page_loads[0]->space();
  fil_space_t *space = fil_space_acquire(space_id);
  page_no_t page_no = m_range.first;
  fil_node_t *node = space->get_file_node(&page_no);
  ut_ad(node != nullptr);
  IORequest request(IORequest::WRITE);
  request.block_size(node->block_size);
  for (auto &page_load : m_page_loads) {
    file::Block *compressed_block = nullptr;
    file::Block *e_block = nullptr;
    size_t page_size = m_page_loads[0]->get_page_size();
    const size_t physical_page_size = m_page_loads[0]->get_page_size();
    page_load->init_for_writing();
    err = fil_prepare_file_for_io(space_id, page_no, &node);
    if (err != DB_SUCCESS) {
      break;
    }
    page_zip_des_t *page_zip = page_load->get_page_zip();
    const os_offset_t offset = page_no * page_size;
    void *buf = (page_zip == nullptr) ? page_load->get_page() : page_zip->data;
    ut_ad(buf != nullptr);
    ut_ad(!buf_page_t::is_zeroes(static_cast<byte *>(buf), page_size));
    {
      ulint buflen = page_size;
      /* Transparent page compression (TPC) is disabled if punch hole is not
      supported. A similar check is done in Fil_shard::do_io(). */
      const bool do_compression =
          space->is_compressed() && (page_zip == nullptr) &&
          IORequest::is_punch_hole_supported() && node->punch_hole;
      if (do_compression) {
        /* @note Compression needs to be done before encryption. */
        /* The page size must be a multiple of the OS punch hole size. */
        ut_ad(buflen % request.block_size() == 0);

        request.compression_algorithm(space->compression_type);
        compressed_block = os_file_compress_page(request, buf, &buflen);
        page_size = buflen;
      }
      if (space->is_encrypted()) {
        request.get_encryption_info().set(space->m_encryption_metadata);
        e_block = os_file_encrypt_page(request, buf, buflen);
      }
    }

    ut_a(node->is_open);
    ut_a(node->size >= page_no);
    SyncFileIO sync_file_io(node->handle.m_file, buf, page_size, offset);
    err = sync_file_io.execute_with_retry(request);
    if (err != DB_SUCCESS) {
      ut_a(err == DB_SUCCESS);
      fil_complete_write(space_id, node);
      break;
    }
    page_no++;
    if (compressed_block != nullptr) {
      file::Block::free(compressed_block);
      const size_t hole_offset = offset + page_size;
      const size_t hole_size = physical_page_size - page_size;
      (void)os_file_punch_hole(node->handle.m_file, hole_offset, hole_size);
    }
    if (e_block != nullptr) {
      file::Block::free(e_block);
    }
    fil_complete_write(space_id, node);
  }
  fil_space_release(space);
  return err;
}

dberr_t Page_extent::bulk_flush() {
#ifdef _WIN32
  return bulk_flush_win();
#elif defined(UNIV_SOLARIS)
  return flush_one_by_one();
#else
  return bulk_flush_linux();
#endif /* _WIN32 */
}

struct Page_load_compare {
  bool operator()(const Page_load *l_page_load, const Page_load *r_page_load) {
    return l_page_load->get_page_no() < r_page_load->get_page_no();
  }
};

dberr_t Page_extent::flush() {
  dberr_t err{DB_SUCCESS};
  const page_no_t n_pages = m_page_loads.size();

  if (n_pages == 0) {
    /* Nothing to do. */
    return err;
  }

  if (!is_fully_used()) {
    const Page_range_t pages = pages_to_free();
    m_is_leaf ? m_btree_load->free_pages_leaf(pages)
              : m_btree_load->free_pages_top(pages);
  }

  std::sort(m_page_loads.begin(), m_page_loads.end(), Page_load_compare());

#ifdef UNIV_DEBUG
  bool in_order = true;
  for (size_t i = m_range.first, j = 0;
       i < m_range.second && j < m_page_loads.size(); ++i, ++j) {
    if (in_order) {
      if (i != m_page_loads[j]->get_page_no()) {
        in_order = false;
      }
    }
  }
  ut_ad(in_order);
#endif /* UNIV_DEBUG */

  for (auto &page_load : m_page_loads) {
    const page_no_t page_no = page_load->get_page_no();
    /* In the debug build we assert, but in the release build we report a
    internal failure. */
    ut_ad(page_no >= m_range.first);
    ut_ad(page_no < m_range.second);
    if (page_no < m_range.first || page_no >= m_range.second) {
      /* The page_no is out of range for the given extent. Report error. */
      return DB_FAIL;
    }
#ifdef UNIV_DEBUG
    {
      const page_id_t page_id = page_load->get_page_id();
      const page_size_t page_size =
          dict_table_page_size(page_load->index()->table);

      /* It would be incorrect to have a dirty version of page_id in the buffer
      pool. Verify this with a debug assert. */
      mtr_t local_mtr;
      local_mtr.start();
      buf_block_t *blk = buf_page_get_gen(
          page_id, page_size, RW_S_LATCH, nullptr,
          Page_fetch::IF_IN_POOL_POSSIBLY_FREED, UT_LOCATION_HERE, &local_mtr);

      /* A clean copy of the page can be there in buffer pool (read ahead
      brings the page to buffer pool). This is OK.  This old copy will be
      evicted after flushing. */
      ut_ad(blk == nullptr || blk->was_freed() || !blk->page.is_dirty());
      local_mtr.commit();
    }
#endif /* UNIV_DEBUG */
  }

  if (m_btree_load->is_tpc_enabled() || m_btree_load->is_tpe_enabled()) {
    err = flush_one_by_one();
  } else {
    err = bulk_flush();
  }

  /* Remove any old copies in the buffer pool. */
  m_btree_load->force_evict(m_range);

#ifdef UNIV_DEBUG
  if (err == DB_SUCCESS) {
    dict_index_t *index = m_page_loads[0]->index();
    for (page_no_t i = m_range.first; i < n_pages; ++i) {
      check_page(index, i);
    }
  }
#endif /* UNIV_DEBUG */
  return err;
}

void Page_extent::destroy_cached() {
  for (auto page_load : m_cached_page_loads) {
    ut_ad(page_load->is_cached());
    ut::delete_(page_load);
  }
  m_cached_page_loads.clear();
}

dberr_t Page_extent::destroy() {
  for (auto page_load : m_page_loads) {
    page_load->free();
    Page_load::drop(page_load);
  }
  m_page_loads.clear();

  return DB_SUCCESS;
}

page_no_t Level_ctx::alloc_page_num() {
  if (m_extent_full) {
    dberr_t err = alloc_extent();
    if (err != DB_SUCCESS) {
      return FIL_NULL;
    }
  }
  page_no_t page_no = m_page_extent->alloc();
  if (page_no == FIL_NULL) {
    dberr_t err = alloc_extent();
    if (err != DB_SUCCESS) {
      return FIL_NULL;
    }
    ut_ad(m_page_extent->is_valid());
    page_no = m_page_extent->alloc();
    ut_ad(page_no != FIL_NULL);
  }
  if (m_page_extent->is_fully_used()) {
    m_extent_full = true;
  }
  m_stat_n_pages++;
  m_btree_load->m_stat_n_pages++;
  ut_ad(page_no != 0);
  ut_ad(page_no != FIL_NULL);
  return page_no;
}

dberr_t Level_ctx::alloc_extent() {
  ut_ad(m_extent_full);

  if (!load_extent_from_cache()) {
    const bool is_leaf = (m_level == 0);
    const bool skip_track = false;
    m_page_extent = Page_extent::create(m_btree_load, is_leaf, skip_track);
  }

  dberr_t err = m_btree_load->alloc_extent(m_page_extent->m_range, m_level);
  if (err != DB_SUCCESS) {
    return err;
  }
  m_page_extent->init();
  ut_ad(m_page_extent->is_valid());
  ut_ad(!m_page_extent->is_fully_used());
  ++m_stat_n_extents;
  m_extent_full = false;
  return err;
}

Level_ctx *Level_ctx::create(dict_index_t *index, size_t level,
                             Btree_load *btree_load) {
  Level_ctx *lvl_ctx = ut::new_withkey<Level_ctx>(UT_NEW_THIS_FILE_PSI_KEY,
                                                  index, level, btree_load);

  return lvl_ctx;
}

void Level_ctx::destroy(Level_ctx *ctx) {
  if (ctx == nullptr) {
    return;
  }
  /* Free cached extents. */
  for (auto cached_extent : ctx->m_cached_extents) {
    ut_ad(cached_extent->m_page_loads.empty());
    cached_extent->destroy_cached();
    ut::delete_(cached_extent);
  }
  ctx->m_cached_extents.clear();
  ut::delete_(ctx);
}

Page_load *Page_load::create(Btree_load *btree_load, Page_extent *page_extent) {
  ut_ad(page_extent->is_valid());
  auto index = btree_load->index();
  auto page_load =
      ut::new_withkey<Page_load>(UT_NEW_THIS_FILE_PSI_KEY, index, btree_load);
  page_load->set_page_extent(page_extent);
  btree_load->add_to_bulk_flusher();
  return page_load;
}

void Page_load::drop(Page_load *page_load) {
  if (page_load == nullptr || page_load->is_cached()) {
    return;
  }
  ut::delete_(page_load);
}

Page_load *Level_ctx::create_page_load() {
  ut_ad(m_page_extent->is_valid());
  const trx_id_t trx_id = m_btree_load->get_trx_id();
  auto page_load = Page_load::create(m_btree_load, m_page_extent);
  page_load->set_trx_id(trx_id);
  page_load->set_page_no(FIL_NULL);
  page_load->set_level(m_level);
  page_load->set_flush_observer(nullptr);
  page_load->set_level_ctx(this);
  ut_ad(m_page_extent != nullptr);
  ut_ad(m_page_extent->is_valid());
  return page_load;
}

void Level_ctx::free_page_load() {
  Page_load::drop(m_page_load);
  m_page_load = nullptr;
}

bool Btree_load::get_cached_range(Page_range_t &page_range, size_t level) {
  if (!cache_ranges()) {
    return false;
  }
  auto &num_cached = level > 0 ? m_num_top_cached : m_num_leaf_cached;
  if (num_cached == 0) {
    return false;
  }

  --num_cached;
  page_range = level > 0 ? m_cached_ranges_top[num_cached]
                         : m_cached_ranges_leaf[num_cached];
  return true;
}

dberr_t Btree_load::fill_cached_range(Page_range_t &page_range, size_t level,
                                      mtr_t *mtr) {
  if (!cache_ranges()) {
    return level > 0
               ? btr_extent_alloc_top(m_index, page_range, mtr, m_fseg_hdr_top)
               : btr_extent_alloc_leaf(m_index, page_range, mtr,
                                       m_fseg_hdr_leaf);
  }

  ut_a(m_max_cached <= S_MAX_CACHED_RANGES);

  if (level > 0) {
    ut_ad(m_num_top_cached == 0);
    m_num_top_cached = 0;
  } else {
    ut_ad(m_num_leaf_cached == 0);
    m_num_leaf_cached = 0;
  }

  dberr_t err = DB_SUCCESS;

  for (size_t index = 0; index < m_max_cached; ++index) {
    if (level > 0) {
      err = btr_extent_alloc_top(m_index, m_cached_ranges_top[index], mtr,
                                 m_fseg_hdr_top);
      ++m_num_top_cached;
    } else {
      err = btr_extent_alloc_leaf(m_index, m_cached_ranges_leaf[index], mtr,
                                  m_fseg_hdr_leaf);
      ++m_num_leaf_cached;
    }
    if (err != DB_SUCCESS) {
      break;
    }
  }

  get_cached_range(page_range, level);
  return err;
}

bool Btree_load::allocate_in_pages() const {
  bool in_pages = false;
  size_t curr_size = srv_buf_pool_curr_size;
  size_t gb = 1024 * 1024 * 1024;
  if (curr_size < gb) {
    in_pages = true;
  } else {
    for (size_t i = 0; i < srv_buf_pool_instances; ++i) {
      buf_pool_t *buf_pool = buf_pool_from_array(i);
      if (buf_pool->curr_size != buf_pool->old_size) {
        /* Buffer pool resize might be in progress. Use pages and not extents.
         */
        in_pages = true;
        break;
      } else {
        /* Over 50 % of the buffer pool is occupied by lock heaps or the
        adaptive hash index or BUF_BLOCK_MEMORY pages. Use pages and not
        extents. */
        if ((UT_LIST_GET_LEN(buf_pool->free) + UT_LIST_GET_LEN(buf_pool->LRU)) <
            buf_pool->curr_size / 2) {
          in_pages = true;
          break;
        }
      }
    }
  }
  return in_pages;
}

dberr_t Btree_load::alloc_extent(Page_range_t &page_range, size_t level) {
  dberr_t err{DB_SUCCESS};
  const space_id_t space_id = m_index->space;
  const page_no_t n_pages = 1;
  bool small_tree = false;
  const ulint n_ext = cache_ranges() ? m_max_cached : 1;
  ulint n_reserved;

  while (m_bulk_flusher.is_full()) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }

  if (get_cached_range(page_range, level)) {
    ++m_stat_n_extents;
    return DB_SUCCESS;
  }

  mtr_t mtr;
  mtr.start();
  mtr.x_lock(dict_index_get_lock(m_index), UT_LOCATION_HERE);
  const bool has_done_reservation = fsp_reserve_free_extents(
      &n_reserved, space_id, n_ext, FSP_NORMAL, &mtr, n_pages);
  if ((has_done_reservation && n_reserved == 0) || allocate_in_pages()) {
    small_tree = true;
  }
  page_t *root = btr_root_get(m_index, &mtr);
  fseg_header_t *seg_header = root + PAGE_HEADER + PAGE_BTR_SEG_LEAF;
  if (level == 0) {
    seg_header = root + PAGE_HEADER + PAGE_BTR_SEG_LEAF;
  } else {
    seg_header = root + PAGE_HEADER + PAGE_BTR_SEG_TOP;
  }
  if (small_tree) {
    const page_size_t page_size = dict_table_page_size(m_index->table);
    fil_space_t *space = fil_space_acquire(space_id);
    fseg_inode_t *seg_inode =
        fseg_inode_get(seg_header, space_id, page_size, &mtr);
    page_no_t page_no =
        fseg_alloc_page_no(space, page_size, seg_inode, FIL_NULL, FSP_NO_DIR,
                           &mtr IF_DEBUG(, has_done_reservation));
    fil_space_release(space);
    page_range.first = page_no;
    page_range.second = page_range.first + 1;
  } else {
    err = fill_cached_range(page_range, level, &mtr);
    ++m_stat_n_extents;
  }
  mtr.commit();
  if (err == DB_SUCCESS) {
    force_evict(page_range);
  }

  if (n_reserved > 0) {
    fil_space_release_free_extents(space_id, n_reserved);
  }
  ut_ad(page_range.first != 0);
  ut_ad(page_range.first != FIL_NULL);
  ut_ad(page_range.second != 0);
  ut_ad(page_range.second != FIL_NULL);
  return err;
}

bool Level_ctx::load_extent_from_cache() {
  if (!m_btree_load->cache_ranges()) {
    return false;
  }
  /* Wait for 1 sec in total with increasing wait interval. */
  size_t max_retry = 30;
  size_t trial = 0;

  for (trial = 0; trial < max_retry; ++trial) {
    for (auto extent : m_cached_extents) {
      if (extent->is_free()) {
        extent->set_state(false);
        m_page_extent = extent;
        m_page_extent->reset_cached_page_loads();
        /* We don't do track_extent(). The extents are directly added
        to flush queue after page_commit if found full. */
        if (trial > 0) {
          ib::info(ER_BULK_LOADER_INFO)
              << "Found cached Extent. Retry count: " << trial;
        }
        return true;
      }
    }
    if (trial < 10) {
      /* First 10 ms: sleep for 1 ms and check. */
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    } else if (trial < 20) {
      /* 10 ms to 100 ms: sleep for 10 ms and check. */
      std::this_thread::sleep_for(std::chrono::milliseconds{10});
    } else {
      /* 100 ms to 1 s: sleep for 100 ms and check. */
      std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
  }
  ib::info(ER_BULK_LOADER_INFO) << "Failed to find free cached Page Extent";
  return false;
}

Page_load *Level_ctx::get_page_load_from_cache() {
  auto &page_loads = m_page_extent->m_cached_page_loads;
  auto &cache_index = m_page_extent->m_next_cached_page_load_index;

  if (cache_index < page_loads.size()) {
    auto page_load = page_loads[cache_index];
    ++cache_index;

    page_load->set_page_no(FIL_NULL);
    return page_load;
  }

  if (m_btree_load->cache_ranges() && m_page_extent->is_cached()) {
    ib::info(ER_BULK_LOADER_INFO) << "Failed to find free cached Page Load";
  }
  return nullptr;
}

void Level_ctx::build_page_cache() {
  ut_ad(m_btree_load->cache_ranges());

  auto &page_loads = m_page_extent->m_cached_page_loads;
  size_t num_page_loads = FSP_EXTENT_SIZE;

  page_loads.clear();
  for (size_t index = 0; index < num_page_loads; index++) {
    auto page_load = create_page_load();
    /* Mark the Page Load as cached. Should not be freed after flush. */
    page_load->set_cached();
    page_loads.push_back(page_load);
  }
  m_page_extent->m_next_cached_page_load_index = 0;
}

void Level_ctx::build_extent_cache() {
  if (!m_btree_load->cache_ranges()) {
    return;
  }
  /* Currently we cache elements twice the maximum flush queue size. The
  cached elements can be reused after the extent is flushed. The flush queue
  is common for all the levels but allocating max for each levels ensure that
  a free element is always available. */
  auto cache_size = 2 * m_btree_load->get_max_flush_queue_size();
  const bool is_leaf = (m_level == 0);
  const bool skip_track = true;

  for (size_t index = 0; index < cache_size; index++) {
    auto page_extent = Page_extent::create(m_btree_load, is_leaf, skip_track);
    /* Mark the extent as cached. Should not be freed after flush. */
    page_extent->set_cached();

    /* Mark the cached entry as free to be used. */
    bool free = true;
    page_extent->set_state(free);

    m_cached_extents.push_back(page_extent);

    m_page_extent = page_extent;
    build_page_cache();
  }
  m_page_extent = nullptr;
}

dberr_t Level_ctx::init() {
  dberr_t er{DB_SUCCESS};
  build_extent_cache();

  if (!load_extent_from_cache()) {
    const bool is_leaf = (m_level == 0);
    const bool skip_track = false;
    m_page_extent = Page_extent::create(m_btree_load, is_leaf, skip_track);
  }

  if (m_page_extent == nullptr) {
    return DB_OUT_OF_MEMORY;
  }

  er = m_btree_load->alloc_extent(m_page_extent->m_range, m_level);
  if (er != DB_SUCCESS) {
    return er;
  }

  m_page_extent->init();
  m_extent_full = false;

  ut_ad(m_page_load == nullptr);
  m_page_load = get_page_load_from_cache();

  if (m_page_load == nullptr) {
    m_page_load = create_page_load();
  }

  page_no_t new_page_no = m_page_extent->alloc();

  if (m_page_extent->is_fully_used()) {
    m_extent_full = true;
  }

  er = m_page_load->init_mem(new_page_no, m_page_extent);
  if (er != DB_SUCCESS) {
    return er;
  }

  return er;
}

[[nodiscard]] buf_block_t *Level_ctx::alloc(
    const page_no_t new_page_no) noexcept {
  const page_id_t new_page_id(m_index->space, new_page_no);
  const page_size_t page_size = dict_table_page_size(m_index->table);
  buf_pool_t *buf_pool = buf_pool_get(new_page_id);
  buf_block_t *block = buf_block_alloc(buf_pool);
  const page_id_t page_id(m_index->space, new_page_no);
  block->page.reset_page_id(page_id);
  block->page.set_page_size(page_size);
  page_t *new_page = buf_block_get_frame(block);
  mach_write_to_4(new_page + FIL_PAGE_OFFSET, block->page.id.page_no());

  if (page_size.is_compressed()) {
    byte *data = buf_buddy_alloc(buf_pool, page_size.physical());
    block->page.zip.data = data;
    page_zip_set_size(&block->page.zip, page_size.physical());
  }
  fsp_init_file_page_low(block);

  page_zip_des_t *page_zip = buf_block_get_page_zip(block);

  if (page_zip) {
    page_create_zip(block, m_index, m_level, 0, nullptr, FIL_PAGE_INDEX);
  } else {
    ut_ad(!dict_index_is_spatial(m_index));
    page_create_low(block, dict_table_is_comp(m_index->table), FIL_PAGE_INDEX);
    btr_page_set_level(new_page, nullptr, m_level, nullptr);
  }

  btr_page_set_next(new_page, page_zip, FIL_NULL, nullptr);
  btr_page_set_prev(new_page, page_zip, FIL_NULL, nullptr);
  btr_page_set_index_id(new_page, page_zip, m_index->id, nullptr);

#ifdef UNIV_DEBUG
  {
    /* Ensure that this page_id is not there in the buffer pool. */
    mtr_t local_mtr;
    local_mtr.start();
    buf_block_t *blk = buf_page_get_gen(page_id, page_size, RW_S_LATCH, nullptr,
                                        Page_fetch::IF_IN_POOL_POSSIBLY_FREED,
                                        UT_LOCATION_HERE, &local_mtr);
    ut_ad(blk == nullptr || blk->was_freed());
    local_mtr.commit();
  }
#endif /* UNIV_DEBUG */
  return block;
}

void Page_load::set_page_no(const page_no_t page_no) {
  ut_ad(m_block == nullptr || m_block->is_memory());
  m_page_no = page_no;
  if (m_block != nullptr) {
    m_block->page.id.set_page_no(page_no);
    mach_write_to_4(m_page + FIL_PAGE_OFFSET, m_block->page.id.page_no());
  }
}

Page_load::Page_load(dict_index_t *index, Btree_load *btree_load)
    : m_index(index),
      m_is_comp(dict_table_is_comp(index->table)),
      m_btree_load(btree_load) {
  m_is_cached.store(false);
}

dberr_t Page_load::init_blob(const page_no_t new_page_no) noexcept {
  ut_ad(m_block == nullptr);
  ut_ad(m_page_extent != nullptr);
  m_page_no = new_page_no;
  const page_id_t new_page_id(m_index->space, new_page_no);
  const page_size_t page_size = dict_table_page_size(m_index->table);
  buf_pool_t *buf_pool = buf_pool_get(new_page_id);
  m_block = buf_block_alloc(buf_pool);
  m_block->page.reset_page_id(new_page_id);
  m_block->page.set_page_size(page_size);
  page_t *new_page = buf_block_get_frame(m_block);
  mach_write_to_4(new_page + FIL_PAGE_OFFSET, m_block->page.id.page_no());
  if (page_size.is_compressed()) {
    byte *data = buf_buddy_alloc(buf_pool, page_size.physical());
    m_block->page.zip.data = data;
    page_zip_set_size(&m_block->page.zip, page_size.physical());
  }
  fsp_init_file_page_low(m_block);
  btr_page_set_next(new_page, nullptr, FIL_NULL, nullptr);
  btr_page_set_prev(new_page, nullptr, FIL_NULL, nullptr);

#ifdef UNIV_DEBUG
  {
    /* Ensure that this page_id is not there in the buffer pool. */
    mtr_t local_mtr;
    local_mtr.start();
    buf_block_t *blk =
        buf_page_get_gen(new_page_id, page_size, RW_S_LATCH, nullptr,
                         Page_fetch::IF_IN_POOL, UT_LOCATION_HERE, &local_mtr);
    ut_ad(blk == nullptr);
    local_mtr.commit();
  }
#endif /* UNIV_DEBUG */
  return DB_SUCCESS;
}

dberr_t Page_load::init_mem(const page_no_t page_no,
                            Page_extent *page_extent) noexcept {
  ut_ad(page_extent != nullptr);
  ut_ad(page_no >= page_extent->m_range.first);
  ut_ad(page_no < page_extent->m_range.second);

  page_t *new_page;
  page_no_t new_page_no;
  buf_block_t *new_block;
  page_zip_des_t *new_page_zip;
  ut_ad(m_heap == nullptr || is_cached());

  m_page_extent = page_extent;

  if (m_heap == nullptr) {
    m_heap = mem_heap_create(1024, UT_LOCATION_HERE);
  } else {
    /* For cached page loader, reuse the same heap. */
    ut_a(is_cached());
    mem_heap_empty(m_heap);
  }

  ut_ad(m_page_no == FIL_NULL);
  m_mtr = nullptr;

  /* Going to use BUF_BLOCK_MEMORY.  Allocate a new page. */
  new_block = m_level_ctx->alloc(page_no);
  new_page_zip = buf_block_get_page_zip(new_block);
  ut_ad(!dict_index_is_spatial(m_index));
  ut_ad(!dict_index_is_sdi(m_index));
  new_page = buf_block_get_frame(new_block);
  new_page_no = page_get_page_no(new_page);

  btr_page_set_next(new_page, new_page_zip, FIL_NULL, nullptr);
  btr_page_set_prev(new_page, new_page_zip, FIL_NULL, nullptr);
  btr_page_set_index_id(new_page, new_page_zip, m_index->id, nullptr);

  if (dict_index_is_sec_or_ibuf(m_index) && !m_index->table->is_temporary() &&
      page_is_leaf(new_page)) {
    page_update_max_trx_id(new_block, nullptr, m_trx_id, nullptr);
  }

  m_block = new_block;
  m_page = new_page;
  m_page_zip = new_page_zip;
  m_page_no = new_page_no;
  m_cur_rec = page_get_infimum_rec(new_page);
  ut_ad(m_is_comp == page_is_comp(new_page));
  m_free_space = page_get_free_space_of_empty(m_is_comp);

  if (ddl::fill_factor == 100 && m_index->is_clustered()) {
    /* Keep default behavior compatible with 5.6 */
    m_reserved_space = dict_index_get_space_reserve();
  } else {
    m_reserved_space = UNIV_PAGE_SIZE * (100 - ddl::fill_factor) / 100;
  }

  m_padding_space =
      UNIV_PAGE_SIZE - dict_index_zip_pad_optimal_page_size(m_index);
  m_heap_top = page_header_get_ptr(new_page, PAGE_HEAP_TOP);
  m_rec_no = page_header_get_field(new_page, PAGE_N_RECS);

  m_last_slotted_rec = page_get_infimum_rec(m_page);
  m_slotted_rec_no = 0;

  m_modified = true;

  ut_d(m_total_data = 0);

  return DB_SUCCESS;
}

dberr_t Page_load::reinit() noexcept {
  btr_page_set_level(m_page, m_page_zip, m_level, m_mtr);
  page_create_empty(m_block, m_index, m_mtr);
  m_cur_rec = page_get_infimum_rec(m_page);
  m_free_space = page_get_free_space_of_empty(m_is_comp);
  m_padding_space =
      UNIV_PAGE_SIZE - dict_index_zip_pad_optimal_page_size(m_index);
  m_heap_top = page_header_get_ptr(m_page, PAGE_HEAP_TOP);
  m_rec_no = page_header_get_field(m_page, PAGE_N_RECS);
  m_last_slotted_rec = page_get_infimum_rec(m_page);
  m_slotted_rec_no = 0;
  m_modified = true;
  btr_page_set_next(m_page, m_page_zip, FIL_NULL, m_mtr);
  btr_page_set_prev(m_page, m_page_zip, FIL_NULL, m_mtr);
  btr_page_set_index_id(m_page, m_page_zip, m_index->id, m_mtr);
  return DB_SUCCESS;
}

dberr_t Page_load::alloc() noexcept {
  ut_ad(m_page_no == FIL_NULL);
  mtr_t alloc_mtr;
  mtr_t mtr;
  mtr.start();

  /* We commit redo log for allocation by a separate mtr,
  because we don't guarantee pages are committed following
  the allocation order, and we will always generate redo log
  for page allocation, even when creating a new tablespace. */
  alloc_mtr.start();

  ulint n_reserved;
  bool success = fsp_reserve_free_extents(&n_reserved, m_index->space, 1,
                                          FSP_NORMAL, &alloc_mtr);
  if (!success) {
    alloc_mtr.commit();
    return DB_OUT_OF_FILE_SPACE;
  }

  /* Allocate a new page. */
  auto new_block =
      btr_page_alloc(m_index, 0, FSP_UP, m_level, &alloc_mtr, &mtr);

  auto new_page = buf_block_get_frame(new_block);

  if (n_reserved > 0) {
    fil_space_release_free_extents(m_index->space, n_reserved);
  }

  m_page_no = new_block->page.id.page_no();
  alloc_mtr.commit();

  auto new_page_zip = buf_block_get_page_zip(new_block);

  if (new_page_zip) {
    const trx_id_t max_trx_id = 0;
    page_create_zip(new_block, m_index, m_level, max_trx_id, &mtr,
                    FIL_PAGE_INDEX);
  } else {
    ut_ad(!dict_index_is_spatial(m_index));
    page_create(new_block, &mtr, dict_table_is_comp(m_index->table),
                FIL_PAGE_INDEX);
  }

  btr_page_set_level(new_page, new_page_zip, m_level, &mtr);
  btr_page_set_index_id(new_page, new_page_zip, m_index->id, &mtr);

  mtr.commit();
  return DB_SUCCESS;
}

dberr_t Page_load::init() noexcept {
  page_t *new_page;
  page_no_t new_page_no;
  buf_block_t *new_block;
  page_zip_des_t *new_page_zip;

  /* Call this function only when mtr is to be used. */
  ut_ad(m_page_no != FIL_NULL);

  ut_ad(m_heap == nullptr);

  m_heap = mem_heap_create(1024, UT_LOCATION_HERE);

  auto mtr_alloc = mem_heap_alloc(m_heap, sizeof(mtr_t));
  auto mtr = new (mtr_alloc) mtr_t();
  mtr->start();
  if (m_flush_observer != nullptr) {
    mtr->set_log_mode(MTR_LOG_NO_REDO);
    mtr->set_flush_observer(m_flush_observer);
  }
  m_mtr = mtr;
  m_mtr->set_modified();

  if (!dict_index_is_online_ddl(m_index)) {
    mtr->x_lock(dict_index_get_lock(m_index), UT_LOCATION_HERE);
  }

  page_id_t page_id(dict_index_get_space(m_index), m_page_no);
  page_size_t page_size(dict_table_page_size(m_index->table));

  new_block =
      buf_page_get(page_id, page_size, RW_X_LATCH, UT_LOCATION_HERE, m_mtr);

  new_page = buf_block_get_frame(new_block);
  new_page_zip = buf_block_get_page_zip(new_block);
  new_page_no = page_get_page_no(new_page);
  ut_ad(m_page_no == new_page_no);

  ut_ad(page_dir_get_n_heap(new_page) == PAGE_HEAP_NO_USER_LOW);

  btr_page_set_level(new_page, nullptr, m_level, m_mtr);

  if (dict_index_is_sec_or_ibuf(m_index) && !m_index->table->is_temporary() &&
      page_is_leaf(new_page)) {
    page_update_max_trx_id(new_block, nullptr, m_trx_id, nullptr);
  }

  m_block = new_block;
  m_page = new_page;
  m_page_zip = new_page_zip;
  m_page_no = new_page_no;
  m_cur_rec = page_get_infimum_rec(new_page);
  ut_ad(m_is_comp == page_is_comp(new_page));
  m_free_space = page_get_free_space_of_empty(m_is_comp);

  if (ddl::fill_factor == 100 && m_index->is_clustered()) {
    /* Keep default behavior compatible with 5.6 */
    m_reserved_space = dict_index_get_space_reserve();
  } else {
    m_reserved_space = UNIV_PAGE_SIZE * (100 - ddl::fill_factor) / 100;
  }

  m_padding_space =
      UNIV_PAGE_SIZE - dict_index_zip_pad_optimal_page_size(m_index);
  m_heap_top = page_header_get_ptr(new_page, PAGE_HEAP_TOP);
  m_rec_no = page_header_get_field(new_page, PAGE_N_RECS);

  m_last_slotted_rec = page_get_infimum_rec(m_page);
  m_slotted_rec_no = 0;

  m_modified = true;

  ut_d(m_total_data = 0);

  return DB_SUCCESS;
}

dberr_t Page_load::insert(const rec_t *rec, Rec_offsets offsets) noexcept {
  ut_ad(m_heap != nullptr);

  auto rec_size = rec_offs_size(offsets);

#ifdef UNIV_DEBUG
  /* Check whether records are in order. */
  if (!page_rec_is_infimum(m_cur_rec)) {
    auto old_rec = m_cur_rec;

    auto old_offsets = rec_get_offsets(
        old_rec, m_index, nullptr, ULINT_UNDEFINED, UT_LOCATION_HERE, &m_heap);

    ut_ad(cmp_rec_rec(rec, old_rec, offsets, old_offsets, m_index,
                      page_is_spatial_non_leaf(old_rec, m_index)) > 0 ||
          (m_index->is_multi_value() &&
           cmp_rec_rec(rec, old_rec, offsets, old_offsets, m_index,
                       page_is_spatial_non_leaf(old_rec, m_index)) >= 0));
  }

  m_total_data += rec_size;
#endif /* UNIV_DEBUG */

  /* 0. Mark space for record as used (checked e.g. in page_rec_set_next). */
  page_header_set_ptr(m_page, nullptr, PAGE_HEAP_TOP, m_heap_top + rec_size);

  /* 1. Copy the record to page. */
  auto insert_rec = rec_copy(m_heap_top, rec, offsets);
  rec_offs_make_valid(insert_rec, m_index, offsets);

  /* 2. Insert the record in the linked list. */
  rec_t *next_rec = page_rec_get_next(m_cur_rec);

  page_rec_set_next(insert_rec, next_rec);
  page_rec_set_next(m_cur_rec, insert_rec);

  /* 3. Set the n_owned field in the inserted record to zero,
  and set the heap_no field. */
  if (m_is_comp) {
    rec_set_n_owned_new(insert_rec, nullptr, 0);
    rec_set_heap_no_new(insert_rec, PAGE_HEAP_NO_USER_LOW + m_rec_no);
  } else {
    rec_set_n_owned_old(insert_rec, 0);
    rec_set_heap_no_old(insert_rec, PAGE_HEAP_NO_USER_LOW + m_rec_no);
  }

  /* 4. Set member variables. */
  auto slot_size = page_dir_calc_reserved_space(m_rec_no + 1) -
                   page_dir_calc_reserved_space(m_rec_no);

  ut_ad(m_free_space >= rec_size + slot_size);
  ut_ad(m_heap_top + rec_size < m_page + UNIV_PAGE_SIZE);

  m_free_space -= rec_size + slot_size;
  m_heap_top += rec_size;
  m_rec_no += 1;
  m_cur_rec = insert_rec;

  m_modified = true;

  return DB_SUCCESS;
}

dberr_t Page_load::insert(const dtuple_t *tuple, const big_rec_t *big_rec,
                          size_t rec_size) noexcept {
  IF_ENABLED("ddl_btree_build_insert_return_interrupt", return DB_INTERRUPTED;)

  /* The memory allocated for temporary record can be reset immediately. We do
  it to avoid repeated malloc because of cumulative allocation of record buffer
  memory. Ideally we should get rid of this allocation and write directly to
  data page. */
  auto saved_top = mem_heap_get_heap_top(m_heap);
  /* Convert tuple to record. */
  auto rec_mem = static_cast<byte *>(mem_heap_alloc(m_heap, rec_size));

  auto rec = rec_convert_dtuple_to_rec(rec_mem, m_index, tuple);

  Rec_offsets offsets{};

  offsets = rec_get_offsets(rec, m_index, offsets, ULINT_UNDEFINED,
                            UT_LOCATION_HERE, &m_heap);

  /* Insert the record.*/
  auto err = insert(rec, offsets);

  if (err != DB_SUCCESS) {
    return err;
  }

  ut_ad(m_modified);

  if (big_rec != nullptr) {
    /* The page must be valid as MTR may be committed during LOB insertion. */
    finish();
    err = store_ext(big_rec, offsets);
  }

  mem_heap_free_heap_top(m_heap, saved_top);
  return err;
}

void Page_load::finish() noexcept {
  ut_ad(!dict_index_is_spatial(m_index));

  if (!m_modified) {
    return;
  }

  ut_ad(m_total_data + page_dir_calc_reserved_space(m_rec_no) <=
        page_get_free_space_of_empty(m_is_comp));

  auto n_rec_to_assign = m_rec_no - m_slotted_rec_no;

  /* Fill slots for non-supremum records if possible.
  Slot for supremum record could store up to
  PAGE_DIR_SLOT_MAX_N_OWNED-1 records. */
  while (n_rec_to_assign >= PAGE_DIR_SLOT_MAX_N_OWNED) {
    static constexpr size_t RECORDS_PER_SLOT =
        (PAGE_DIR_SLOT_MAX_N_OWNED + 1) / 2;

    for (size_t i = 0; i < RECORDS_PER_SLOT; ++i) {
      m_last_slotted_rec = page_rec_get_next(m_last_slotted_rec);
    }
    m_slotted_rec_no += RECORDS_PER_SLOT;

    /* Reserve next slot (must be done before slot is used). */
    auto n_slots = page_dir_get_n_slots(m_page);
    page_dir_set_n_slots(m_page, nullptr, n_slots + 1);

    /* Fill the slot data. */
    auto slot = page_dir_get_nth_slot(m_page, n_slots - 1);
    page_dir_slot_set_rec(slot, m_last_slotted_rec);
    page_dir_slot_set_n_owned(slot, nullptr, RECORDS_PER_SLOT);

    n_rec_to_assign -= RECORDS_PER_SLOT;
  }

  /* Assign remaining records to slot with supremum record. */
  auto n_slots = page_dir_get_n_slots(m_page);
  auto slot = page_dir_get_nth_slot(m_page, n_slots - 1);
  auto sup_rec = page_get_supremum_rec(m_page);

  page_dir_slot_set_rec(slot, sup_rec);
  page_dir_slot_set_n_owned(slot, nullptr, n_rec_to_assign + 1);

  page_header_set_ptr(m_page, nullptr, PAGE_HEAP_TOP, m_heap_top);
  page_dir_set_n_heap(m_page, nullptr, PAGE_HEAP_NO_USER_LOW + m_rec_no);
  page_header_set_field(m_page, nullptr, PAGE_N_RECS, m_rec_no);
  page_header_set_ptr(m_page, nullptr, PAGE_LAST_INSERT, m_cur_rec);
  page_header_set_field(m_page, nullptr, PAGE_DIRECTION, PAGE_RIGHT);
  page_header_set_field(m_page, nullptr, PAGE_N_DIRECTION, 0);
  m_modified = false;
  ut_ad(page_validate(m_page, m_index));
}

dberr_t Page_load::commit() noexcept {
  /* It is assumed that finish() was called before commit */
  ut_a(!m_modified);
  ut_ad(page_validate(m_page, m_index));
  ut_a(m_rec_no > 0);

  /* Set no free space left and no buffered changes in ibuf. */
  if (!m_index->is_clustered() && !m_index->table->is_temporary() &&
      page_is_leaf(m_page)) {
    ibuf_set_bitmap_for_bulk_load(m_block, ddl::fill_factor == 100);
  }
  ut_ad(btr_page_get_index_id(m_page) == m_index->id);
  if (m_mtr != nullptr) {
    m_mtr->commit();
  } else {
    ut_ad(m_page_extent != nullptr);
    m_page_extent->append(this);
    if (m_page_extent->is_fully_used()) {
      m_page_extent = nullptr;
    }
  }
  if (m_btree_load != nullptr) {
    if ((m_level + 1) != m_btree_load->m_last_page_nos.size()) {
      m_btree_load->m_last_page_nos[m_level] = get_page_no();
    }
  }
  return DB_SUCCESS;
}

void Page_load::rollback() noexcept {}

bool Page_load::compress() noexcept {
  ut_ad(!m_modified);
  ut_ad(m_page_zip != nullptr);

  return page_zip_compress(m_page_zip, m_page, m_index, page_zip_level,
                           nullptr);
}

dtuple_t *Page_load::get_node_ptr() noexcept {
  /* Create node pointer */
  auto first_rec = page_rec_get_next(page_get_infimum_rec(m_page));
  ut_a(page_rec_is_user_rec(first_rec));

  auto node_ptr =
      dict_index_build_node_ptr(m_index, first_rec, m_page_no, m_heap, m_level);

  return node_ptr;
}

void Page_load::split(Page_load &new_page_loader) noexcept {
  auto split_point = get_split_rec();

  new_page_loader.copy_records(split_point.m_rec);
  split_trim(split_point);

  ut_ad(new_page_loader.m_modified);
  ut_ad(m_modified);
}

Page_load::Split_point Page_load::get_split_rec() noexcept {
  ut_a(m_rec_no >= 2);
  ut_a(m_page_zip != nullptr);
  ut_a(page_get_free_space_of_empty(m_is_comp) > m_free_space);

  auto total_used_size = page_get_free_space_of_empty(m_is_comp) - m_free_space;

  size_t n_recs{};
  Rec_offsets offsets{};
  size_t total_recs_size{};

  auto rec = page_get_infimum_rec(m_page);

  do {
    rec = page_rec_get_next(rec);
    ut_ad(page_rec_is_user_rec(rec));

    offsets = rec_get_offsets(rec, m_index, offsets, ULINT_UNDEFINED,
                              UT_LOCATION_HERE, &m_heap);
    total_recs_size += rec_offs_size(offsets);
    n_recs++;
  } while (total_recs_size + page_dir_calc_reserved_space(n_recs) <
           total_used_size / 2);

  /* Keep at least one record on left page */
  if (page_rec_is_infimum(page_rec_get_prev(rec))) {
    rec = page_rec_get_next(rec);
    ut_ad(page_rec_is_user_rec(rec));
  } else {
    /* rec is to be moved, and this is used as number of records before split
     */
    --n_recs;
  }

  return Split_point{rec, n_recs};
}

void Page_load::print_child_page_nos() noexcept {
  if (m_level == 0) {
    return;
  }

  Rec_offsets offsets{};
  auto inf_rec = page_get_infimum_rec(m_page);
  auto rec = page_rec_get_next_const(inf_rec);
  ut_ad(page_rec_is_user_rec(rec));
  do {
    offsets = rec_get_offsets(rec, m_index, offsets, ULINT_UNDEFINED,
                              UT_LOCATION_HERE, &(m_heap));
    rec = page_rec_get_next_const(rec);
  } while (!page_rec_is_supremum(rec));
}

size_t Page_load::copy_all(const page_t *src_page) noexcept {
  auto inf_rec = page_get_infimum_rec(src_page);
  auto first_rec = page_rec_get_next_const(inf_rec);
  ut_ad(page_rec_is_user_rec(first_rec));
  const size_t n_recs = copy_records(first_rec);
  ut_ad(m_modified);
  return n_recs;
}

size_t Page_load::copy_to(std::vector<Page_load *> &to_pages) {
  auto src_page = get_page();
  auto inf_rec = page_get_infimum_rec(src_page);
  auto first_rec = page_rec_get_next_const(inf_rec);
  const size_t n_recs = page_get_n_recs(src_page);
  const size_t n_pages = to_pages.size();
  const size_t rec_per_page = (n_recs + n_pages) / n_pages;
  Rec_offsets offsets{};
  const rec_t *rec = first_rec;

  size_t rec_count = 1;
  size_t i = 0;
  do {
    offsets = rec_get_offsets(rec, m_index, offsets, ULINT_UNDEFINED,
                              UT_LOCATION_HERE, &m_heap);
    to_pages[i]->insert(rec, offsets);
    rec = page_rec_get_next_const(rec);
    if (rec_count % rec_per_page == 0) {
      ++i;
      ut_a(i < to_pages.size());
    }
    rec_count++;
    ut_a(rec_count <= n_recs);
  } while (!page_rec_is_supremum(rec));
  return rec_count;
}

size_t Page_load::copy_records(const rec_t *first_rec) noexcept {
  Rec_offsets offsets{};
  const rec_t *rec = first_rec;

  size_t n_recs{};
  ut_ad(page_rec_is_user_rec(rec));

  do {
    offsets = rec_get_offsets(rec, m_index, offsets, ULINT_UNDEFINED,
                              UT_LOCATION_HERE, &m_heap);
    insert(rec, offsets);
    rec = page_rec_get_next_const(rec);
    n_recs++;
  } while (!page_rec_is_supremum(rec));

  ut_ad(m_rec_no > 0);
  return n_recs;
}

void Page_load::split_trim(const Split_point &split_point) noexcept {
  /* Suppose before copyOut, we have 5 records on the page:
  infimum->r1->r2->r3->r4->r5->supremum, and r3 is the split rec.

  after copyOut, we have 2 records on the page:
  infimum->r1->r2->supremum. slot adjustment is not done. */

  /* Set number of user records. */
  auto new_rec_no = split_point.m_n_rec_before;
  ut_a(new_rec_no > 0);

  /* Set last record's next in page */
  rec_t *new_last_user_rec = page_rec_get_prev(split_point.m_rec);
  page_rec_set_next(new_last_user_rec, page_get_supremum_rec(m_page));

  /* Set related members */
  auto old_heap_top = m_heap_top;

  Rec_offsets offsets{};
  offsets = rec_get_offsets(new_last_user_rec, m_index, offsets,
                            ULINT_UNDEFINED, UT_LOCATION_HERE, &m_heap);
  m_heap_top = rec_get_end(new_last_user_rec, offsets);

  m_free_space +=
      (old_heap_top - m_heap_top) + (page_dir_calc_reserved_space(m_rec_no) -
                                     page_dir_calc_reserved_space(new_rec_no));
  ut_ad(m_free_space > 0);

  m_cur_rec = new_last_user_rec;
  m_rec_no = new_rec_no;

  ut_d(m_total_data -= old_heap_top - m_heap_top);

  /* Invalidate all slots except infimum. */
  auto n_slots = page_dir_get_n_slots(m_page);

  for (size_t slot_idx = 1; slot_idx < n_slots; ++slot_idx) {
    auto slot = page_dir_get_nth_slot(m_page, slot_idx);
    page_dir_slot_set_n_owned(slot, nullptr, 0);
  }

  page_dir_set_n_slots(m_page, nullptr, 2);

  /* No records assigned to slots. */
  m_last_slotted_rec = page_get_infimum_rec(m_page);
  m_slotted_rec_no = 0;

  m_modified = true;
}

void Page_load::set_next(page_no_t next_page_no) noexcept {
  page_zip_des_t *page_zip = get_page_zip();
  btr_page_set_next(m_page, page_zip, next_page_no, m_mtr);
}

void Page_load::set_prev(page_no_t prev_page_no) noexcept {
  page_zip_des_t *page_zip = get_page_zip();
  btr_page_set_prev(m_page, page_zip, prev_page_no, m_mtr);
}

bool Page_load::is_space_available(size_t rec_size) const noexcept {
  auto slot_size = page_dir_calc_reserved_space(m_rec_no + 1) -
                   page_dir_calc_reserved_space(m_rec_no);

  auto required_space = rec_size + slot_size;

  if (required_space > m_free_space) {
    ut_a(m_rec_no > 0);
    return false;
  }

  /* Fillfactor & Padding apply to both leaf and non-leaf pages.
  Note: we keep at least 2 records in a page to avoid B-tree level
  growing too high. */
  if (m_rec_no >= 2 && ((m_page_zip == nullptr &&
                         m_free_space - required_space < m_reserved_space) ||
                        (m_page_zip != nullptr &&
                         m_free_space - required_space < m_padding_space))) {
    return false;
  }

  return true;
}

bool Page_load::need_ext(const dtuple_t *tuple,
                         size_t rec_size) const noexcept {
  return page_zip_rec_needs_ext(rec_size, m_is_comp, dtuple_get_n_fields(tuple),
                                m_block->page.size);
}

dberr_t Page_load::store_ext(const big_rec_t *big_rec,
                             Rec_offsets offsets) noexcept {
  ut_ad(m_index->is_clustered());

  /* Note: not all fields are initialized in btr_pcur. */
  btr_pcur_t btr_pcur;
  btr_pcur.m_pos_state = BTR_PCUR_IS_POSITIONED;
  btr_pcur.m_latch_mode = BTR_MODIFY_LEAF;
  btr_pcur.m_btr_cur.index = m_index;

  page_cur_t *page_cur = &btr_pcur.m_btr_cur.page_cur;
  page_cur->index = m_index;
  page_cur->rec = m_cur_rec;
  page_cur->offsets = offsets;
  page_cur->block = m_block;

  lob::Lob_ctx lob_ctx;
  lob_ctx.m_btree_load = m_btree_load;
  dberr_t err = lob::btr_store_big_rec_extern_fields(
      lob_ctx, &btr_pcur, nullptr, offsets, big_rec, nullptr,
      lob::OPCODE_INSERT_BULK);

  ut_ad(page_offset(m_cur_rec) == page_offset(page_cur->rec));

  m_btree_load->blob()->flush_index_extents();
  m_btree_load->blob()->clear_cache();

  /* Reset m_block and m_cur_rec from page cursor, because
  block may be changed during blob insert. */
  m_block = page_cur->block;
  m_cur_rec = page_cur->rec;
  m_page = buf_block_get_frame(m_block);

  return err;
}

void Page_load::release() noexcept {}

void Page_load::latch() noexcept {
  m_mtr->start();
  ut_ad(m_flush_observer != nullptr);
  m_mtr->set_log_mode(MTR_LOG_NO_REDO);
  m_mtr->set_flush_observer(m_flush_observer);
  const space_id_t space_id = space();
  const page_id_t page_id(space_id, m_page_no);
  const page_size_t page_size = m_index->get_page_size();
  m_block =
      buf_page_get(page_id, page_size, RW_X_LATCH, UT_LOCATION_HERE, m_mtr);
  m_page = buf_block_get_frame(m_block);
  m_page_zip = buf_block_get_page_zip(m_block);
}

#ifdef UNIV_DEBUG
bool Page_load::is_index_locked() noexcept {
  return (m_mtr == nullptr) ? false
                            : (dict_index_is_online_ddl(m_index) &&
                               m_mtr->memo_contains_flagged(
                                   dict_index_get_lock(m_index),
                                   MTR_MEMO_X_LOCK | MTR_MEMO_SX_LOCK));
}
#endif /* UNIV_DEBUG */

dberr_t Btree_load::page_split(Page_load *page_loader,
                               Page_load *next_page_load) noexcept {
  ut_ad(page_loader->is_table_compressed());

  /* 1. Check if we have only one user record on the page. */
  if (page_loader->get_rec_no() <= 1) {
    return DB_TOO_BIG_RECORD;
  }

  const size_t level = page_loader->get_level();
  auto lvl_ctx = m_level_ctxs[level];
  page_no_t new_page_no = lvl_ctx->alloc_page_num();
  ut_ad(new_page_no != FIL_NULL);

  auto split_page = lvl_ctx->get_page_load_from_cache();

  if (split_page == nullptr) {
    split_page = lvl_ctx->create_page_load();
  }

  auto err = split_page->init_mem(new_page_no, lvl_ctx->m_page_extent);

  if (err != DB_SUCCESS) {
    return err;
  }

  page_loader->split(*split_page);
  split_page->finish();
  page_loader->finish();

  err = page_commit(page_loader, split_page, true);
  if (err != DB_SUCCESS) {
    split_page->rollback();
    return err;
  }

  err = page_commit(split_page, next_page_load, true);
  if (err != DB_SUCCESS) {
    split_page->rollback();
    return err;
  }
  return err;
}

dberr_t Btree_load::page_commit(Page_load *page_loader,
                                Page_load *next_page_loader,
                                bool insert_father) noexcept {
  /* Set page links */
  if (next_page_loader != nullptr) {
    ut_ad(page_loader->get_level() == next_page_loader->get_level());
    page_loader->set_next(next_page_loader->get_page_no());
    next_page_loader->set_prev(page_loader->get_page_no());
  } else {
    /* Suppose a page is released and latched again, we need to
    mark it modified in mini-transaction.  */
    page_loader->set_next(FIL_NULL);
  }

  /* Assert that no locks are held during bulk load operation
  in case of a online ddl operation. Insert thread acquires index->lock
  to check the online status of index. During bulk load index,
  there are no concurrent insert or reads and hence, there is no
  need to acquire a lock in that case. */
  ut_ad(!page_loader->is_index_locked());

  IF_ENABLED("ddl_btree_build_sleep",
             std::this_thread::sleep_for(std::chrono::seconds{1});)

  /* Compress page if it's a compressed table. */
  if (page_loader->is_table_compressed() && !page_loader->compress()) {
    return page_split(page_loader, next_page_loader);
  }

  /* Insert node pointer to father page. */
  if (insert_father) {
    auto node_ptr = page_loader->get_node_ptr();
    const dberr_t err = insert(node_ptr, page_loader->get_level() + 1);

    if (err != DB_SUCCESS) {
      return err;
    }
  }

  /* Commit mtr. */
  page_loader->commit();
  return DB_SUCCESS;
}

void Btree_load::log_free_check() noexcept {
  if (log_free_check_is_required()) {
    release();

    ::log_free_check();

    latch();
  }
}

Btree_load::Btree_load(dict_index_t *index, trx_t *trx,
                       Flush_observer *observer) noexcept
    : m_index(index),
      m_trx(trx),
      m_flush_observer(observer),
      m_compare_key(m_index, nullptr, !m_index->is_clustered()),
      m_page_size(dict_table_page_size(m_index->table)) {
  ut_d(fil_space_inc_redo_skipped_count(m_index->space));
  ut_d(m_index_online = m_index->online_status);
  m_bulk_flusher.start();
  /* The observer may or may not be nullptr. */
  ut_ad(trx->flush_observer == observer);
  m_max_cached = 0;
}

trx_id_t Btree_load::get_trx_id() const { return m_trx->id; }

Btree_load::~Btree_load() noexcept {
  ut_d(fil_space_dec_redo_skipped_count(m_index->space));
  Blob_load::destroy(m_blob_load);
  for (auto level_ctx : m_level_ctxs) {
    Level_ctx::destroy(level_ctx);
  }
  mem_heap_free(m_heap_order);
}

void Btree_load::release() noexcept { /* Nothing to do here. */
}

void Btree_load::latch() noexcept { /* Nothing to do here. */
}

dberr_t Btree_load::prepare_space(Page_load *&page_loader, size_t level,
                                  size_t rec_size) noexcept {
  if (page_loader->is_space_available(rec_size)) {
    return DB_SUCCESS;
  }

  /* Finish page modifications. */
  page_loader->finish();

  IF_ENABLED("ddl_btree_build_oom", return DB_OUT_OF_MEMORY;)

  auto lvl_ctx = m_level_ctxs[level];
  const page_no_t new_page_no = lvl_ctx->alloc_page_num();
  ut_ad(new_page_no != FIL_NULL);

  /* Create a sibling page_loader. */
  auto sibling_page_loader = lvl_ctx->get_page_load_from_cache();

  if (sibling_page_loader == nullptr) {
    sibling_page_loader = lvl_ctx->create_page_load();
  }

  if (sibling_page_loader == nullptr) {
    return DB_OUT_OF_MEMORY;
  }

  {
    auto err =
        sibling_page_loader->init_mem(new_page_no, lvl_ctx->m_page_extent);

    if (err != DB_SUCCESS) {
      Page_load::drop(sibling_page_loader);
      return err;
    }
  }

  /* It is unsafe to access uncached page extent after commit. */
  auto page_extent = page_loader->m_page_extent;
  bool extent_cached = (page_extent != nullptr && page_extent->is_cached());

  /* Commit page bulk. */
  {
    auto err = page_commit(page_loader, sibling_page_loader, true);

    if (err != DB_SUCCESS) {
      sibling_page_loader->rollback();
      Page_load::drop(sibling_page_loader);
      return err;
    }
  }

  /* Set new page bulk to page_loaders. */
  ut_a(sibling_page_loader->get_level() <= m_root_level);
  lvl_ctx->set_current_page_load(sibling_page_loader);
  page_loader = sibling_page_loader;
  m_last_page_nos[level] = new_page_no;

  /* If the cached extent for the page is full, add to flush queue. */
  if (extent_cached && page_extent->is_page_loads_full()) {
    ut_ad(!is_extent_tracked(page_extent));
    ut_ad(sibling_page_loader->m_page_extent != page_extent);
    add_to_bulk_flusher(page_extent);
  }
  return DB_SUCCESS;
}

void Btree_load::add_to_bulk_flusher(Page_extent *page_extent) {
  m_bulk_flusher.add(page_extent);
}

void Btree_load::add_to_bulk_flusher(bool finish) {
  const size_t n = m_extents_tracked.size();
  for (size_t i = 0; i < n; ++i) {
    auto page_extent = m_extents_tracked.front();
    m_extents_tracked.pop_front();
    if (page_extent->is_page_loads_full() || finish) {
      m_bulk_flusher.add(page_extent);
    } else {
      m_extents_tracked.push_back(page_extent);
    }
  }
}

dberr_t Btree_load::insert(Page_load *page_loader, dtuple_t *tuple,
                           big_rec_t *big_rec, size_t rec_size) noexcept {
  if (big_rec != nullptr) {
    ut_a(m_index->is_clustered());
    ut_a(page_loader->get_level() == 0);
    ut_a(page_loader == get_level(0)->get_page_load());
  }
  auto err = page_loader->insert(tuple, big_rec, rec_size);
  return err;
}

dberr_t Btree_load::insert(dtuple_t *tuple, size_t level) noexcept {
  bool is_left_most{};
  dberr_t err{DB_SUCCESS};

  /* Check if data is inserted in sorted order . */
  if (m_check_order && level == 0) {
    if (m_prev_tuple != nullptr) {
      const auto cmp = m_compare_key(m_prev_tuple->fields, tuple->fields);
      if (cmp >= 0) {
        return DB_DATA_NOT_SORTED;
      }
    }
    mem_heap_empty(m_heap_order);
    m_prev_tuple = tuple->deep_copy(m_heap_order);
  }

  if (is_new_level(level)) {
    IF_ENABLED("ddl_btree_build_oom", return DB_OUT_OF_MEMORY;)

    auto lvl_ctx = Level_ctx::create(m_index, level, this);
    if (lvl_ctx == nullptr) {
      return DB_OUT_OF_MEMORY;
    }

    err = lvl_ctx->init();
    if (err != DB_SUCCESS) {
      return err;
    }

    auto page_loader = lvl_ctx->get_page_load();

    DEBUG_SYNC_C("bulk_load_insert");

    m_level_ctxs.push_back(lvl_ctx);
    ut_a(level + 1 == m_level_ctxs.size());
    m_root_level = level;
    is_left_most = true;
    const page_no_t root_page_no = page_loader->get_page_no();
    m_first_page_nos.push_back(root_page_no);
    m_last_page_nos.push_back(root_page_no);
  }

  auto page_loader = get_level(level)->get_page_load();

  if (is_left_most && level > 0 && page_loader->get_rec_no() == 0) {
    /* The node pointer must be marked as the predefined minimum
    record,     as there is no lower alphabetical limit to records in
    the leftmost node of a level: */
    const auto info_bits = dtuple_get_info_bits(tuple) | REC_INFO_MIN_REC_FLAG;
    dtuple_set_info_bits(tuple, info_bits);
  }

  big_rec_t *big_rec{};
  auto rec_size = rec_get_converted_size(m_index, tuple);

  if (page_loader->need_ext(tuple, rec_size)) {
    /* The record is so big that we have to store some fields
    externally on separate database pages */
    big_rec = dtuple_convert_big_rec(m_index, nullptr, tuple);
    if (big_rec == nullptr) {
      if (level > 0) {
        page_loader->release();
      }
      return DB_TOO_BIG_RECORD;
    }

    rec_size = rec_get_converted_size(m_index, tuple);
  }

  if (page_loader->is_table_compressed() &&
      page_zip_is_too_big(m_index, tuple)) {
    err = DB_TOO_BIG_RECORD;
  } else {
    err = prepare_space(page_loader, level, rec_size);

    if (err == DB_SUCCESS) {
      IF_ENABLED(
          "ddl_btree_build_too_big_record", static int rec_cnt = 0;

          if (++rec_cnt == 10) {
            rec_cnt = 0;
            if (big_rec != nullptr) {
              dtuple_convert_back_big_rec(tuple, big_rec);
            }
            if (level > 0) {
              page_loader->release();
            }
            return DB_TOO_BIG_RECORD;
          })

      err = insert(page_loader, tuple, big_rec, rec_size);
    }
  }

  if (big_rec != nullptr) {
    dtuple_convert_back_big_rec(tuple, big_rec);
  }
  if (level > 0) {
    page_loader->release();
  }
  return err;
}

dberr_t Btree_load::finalize_page_loads(dberr_t err,
                                        page_no_t &last_page_no) noexcept {
  ut_a(last_page_no == FIL_NULL);
  ut_a(m_root_level + 1 == m_level_ctxs.size());

  /* Finish all page bulks */
  for (size_t level = 0; level <= m_root_level; level++) {
    auto lvl_ctx = get_level(level);
    if (err == DB_SUCCESS) {
      auto page_loader = lvl_ctx->get_page_load();
      /* It is unsafe to access uncached page extent after commit. */
      auto page_extent = page_loader->m_page_extent;
      bool extent_cached = (page_extent != nullptr && page_extent->is_cached());

      page_loader->finish();
      err = page_commit(page_loader, nullptr, level != m_root_level);

      /* Add the last extent to flush queue. */
      if (extent_cached) {
        add_to_bulk_flusher(page_extent);
      }
    } else {
      lvl_ctx->free_page_load();
    }
  }
  last_page_no = m_last_page_nos[m_root_level];
  return err;
}

dberr_t Btree_load::load_root_page(page_no_t last_page_no) noexcept {
  ut_ad(last_page_no != FIL_NULL);

  page_id_t page_id(dict_index_get_space(m_index), last_page_no);
  page_size_t page_size(dict_table_page_size(m_index->table));
  page_no_t page_no = dict_index_get_page(m_index);

  /* Load the correct root page. */
  Page_load page_loader(m_index, get_trx_id(), page_no, m_root_level,
                        m_flush_observer, this);

  mtr_t mtr;
  mtr.start();
  mtr.x_lock(dict_index_get_lock(m_index), UT_LOCATION_HERE);

  auto last_block = btr_block_get(page_id, page_size, RW_X_LATCH,
                                  UT_LOCATION_HERE, m_index, &mtr);

  auto last_page = buf_block_get_frame(last_block);

  /* Copy last page to root page. */
  auto err = page_loader.init();

  if (err == DB_SUCCESS) {
    size_t n_recs = page_loader.copy_all(last_page);
    ut_a(n_recs > 0);
    page_loader.finish();
    /* Remove last page. */
    btr_page_free_low(m_index, last_block, m_root_level, &mtr);
    /* Do not flush the last page. */
    last_block->page.m_flush_observer = nullptr;
    mtr.commit();
    err = page_commit(&page_loader, nullptr, false);
    ut_a(err == DB_SUCCESS);
  } else {
    mtr.commit();
  }
  return err;
}

#ifdef UNIV_DEBUG
void Btree_load::print_pages_in_level(const size_t level) const {
  const size_t root_level = m_first_page_nos.size() - 1;
  const bool is_root_level = (level == root_level);
  page_no_t page_no =
      is_root_level ? dict_index_get_page(m_index) : m_first_page_nos[level];
  const page_size_t page_size(dict_table_page_size(m_index->table));
  mtr_t mtr;
  while (page_no != FIL_NULL) {
    mtr.start();
    const page_id_t page_id(m_index->space, page_no);
    buf_block_t *block = btr_block_get(page_id, page_size, RW_S_LATCH,
                                       UT_LOCATION_HERE, m_index, &mtr);
    const page_no_t next_page_no = block->get_next_page_no();
    const byte *frame = buf_block_get_frame(block);
    size_t n_recs = page_header_get_field(frame, PAGE_N_RECS);
    (void)n_recs;
    page_no = next_page_no;
    mtr.commit();
  }
  {
    mtr.start();
    const page_no_t last_page_no = m_first_page_nos[root_level];
    const page_id_t page_id(m_index->space, last_page_no);
    buf_block_t *block = btr_block_get(page_id, page_size, RW_S_LATCH,
                                       UT_LOCATION_HERE, m_index, &mtr);
    const page_no_t next_page_no = block->get_next_page_no();
    (void)next_page_no;
    const byte *frame = buf_block_get_frame(block);
    size_t n_recs = page_header_get_field(frame, PAGE_N_RECS);
    (void)n_recs;
    mtr.commit();
  }
}
#endif /* UNIV_DEBUG */

dberr_t Btree_load::finish(dberr_t err, const bool subtree) noexcept {
  ut_ad(!m_index->table->is_temporary());
  /* Assert that the index online status has not changed */

  ut_ad(m_index->online_status == m_index_online || err != DB_SUCCESS);
  if (m_level_ctxs.empty()) {
    /* The table is empty. The root page of the index tree
    is already in a consistent state. No need to flush. */
    return err;
  }

  page_no_t last_page_no{FIL_NULL};

  err = finalize_page_loads(err, last_page_no);

  if (m_blob_load != nullptr) {
    /* Ensure that all pages of type BUF_BLOCK_MEMORY is flushed here. */
    /* First complete all blob page flushes. */
    const dberr_t err2 = m_blob_load->finish();
    if (err2 != DB_SUCCESS && err == DB_SUCCESS) {
      err = err2;
    }
  }

  add_to_bulk_flusher(true);
  m_bulk_flusher.wait_to_stop();

  /* @note After this point, the bulk loaded pages can be accessed using
  regular mtr via buffer pool. */

  if (err == DB_SUCCESS) {
    if (subtree == false) {
      err = load_root_page(last_page_no);
    }
  }

  /* Ensure that remaining pages modified without redo log is flushed here. */
  ut_d(dict_sync_check check(true));
  ut_ad(!sync_check_iterate(check));
  return err;
}

/** The transaction interrupted check is expensive, we check after this
many rows. */
static constexpr uint64_t TRX_INTERRUPTED_CHECK = 25000;

dberr_t Btree_load::build(Cursor &cursor) noexcept {
  dberr_t err;
  dtuple_t *dtuple{};
  uint64_t interrupt_check{};

  if (m_blob_load == nullptr) {
    m_blob_load = Blob_load::create(this);
  }

  while ((err = cursor.fetch(dtuple)) == DB_SUCCESS) {
    if (cursor.duplicates_detected()) {
      err = DB_DUPLICATE_KEY;
      break;
    }

    err = insert(dtuple, 0);

    if (err != DB_SUCCESS) {
      return err;
    }

    if (allocate_in_pages()) {
      m_blob_load->finish();
    }

    err = cursor.next();

    if (err != DB_SUCCESS) {
      break;
    }

    ++m_n_recs;

    IF_ENABLED("ddl_btree_load_interrupt",
               interrupt_check = TRX_INTERRUPTED_CHECK;);

    if (!(interrupt_check++ % TRX_INTERRUPTED_CHECK) && m_trx != nullptr &&
        trx_is_interrupted(m_trx)) {
      err = DB_INTERRUPTED;
      break;
    }
  }
  return err == DB_END_OF_INDEX ? DB_SUCCESS : err;
}

std::ostream &Btree_load::print_left_pages(std::ostream &out) const {
  out << "[Leftmost Pages: ";
  for (const page_no_t page_no : m_first_page_nos) {
    out << page_no << ",";
  }
  out << "]";
  return out;
}

std::ostream &Btree_load::print_right_pages(std::ostream &out) const {
  out << "[Rightmost Pages: ";
  for (const page_no_t page_no : m_last_page_nos) {
    out << page_no << ",";
  }
  out << "]";
  return out;
}

#ifdef UNIV_DEBUG
void Btree_load::print_tree_pages() const {
  for (size_t level = 0; level < m_level_ctxs.size(); ++level) {
    print_pages_in_level(level);
  }
}
#endif /* UNIV_DEBUG */

void Page_load::set_min_rec_flag(mtr_t *mtr) {
  rec_t *first_rec = page_rec_get_next(page_get_infimum_rec(m_page));
  btr_set_min_rec_mark(first_rec, mtr);
}

[[nodiscard]] page_no_t Btree_load::get_top_page() noexcept {
  page_no_t page_no;
  if (likely(is_top_page_available())) {
    page_no = m_page_range_top.first++;
  } else {
    mtr_t mtr;
    mtr.start();
    dberr_t err =
        btr_extent_alloc_top(m_index, m_page_range_top, &mtr, m_fseg_hdr_top);
    if (err != DB_SUCCESS) {
      page_no = FIL_NULL;
    } else {
      force_evict(m_page_range_top);
      page_no = m_page_range_top.first++;
    }
    mtr.commit();
  }
  return page_no;
}

[[nodiscard]] page_no_t Btree_load::get_leaf_page() noexcept {
  page_no_t page_no;
  if (likely(is_leaf_page_available())) {
    page_no = m_page_range_leaf.first++;
  } else {
    mtr_t mtr;
    mtr.start();
    dberr_t err = btr_extent_alloc_leaf(m_index, m_page_range_leaf, &mtr,
                                        m_fseg_hdr_leaf);
    if (err != DB_SUCCESS) {
      page_no = FIL_NULL;
    } else {
      force_evict(m_page_range_leaf);
      page_no = m_page_range_leaf.first++;
    }
    mtr.commit();
  }
  return page_no;
}

void Btree_load::force_evict(const Page_range_t &range) {
  const space_id_t space_id = dict_index_get_space(m_index);
  const page_size_t page_size(dict_table_page_size(m_index->table));

  for (page_no_t p_no = range.first; p_no < range.second; ++p_no) {
    const page_id_t page_id(space_id, p_no);
    buf_page_force_evict(page_id, page_size);
  }
}

void Btree_load::free_pages_leaf(const Page_range_t &range) {
#ifdef UNIV_DEBUG
  mtr_t mtr;
  mtr_start(&mtr);
  page_t *root = btr_root_get(m_index, &mtr);
  fseg_header_t *seg_header = root + PAGE_HEADER + PAGE_BTR_SEG_LEAF;
  ut_ad(memcmp(m_fseg_hdr_leaf, seg_header, FSEG_HEADER_SIZE) == 0);
  mtr_commit(&mtr);
#endif
  free_pages(range, m_fseg_hdr_leaf);
}

void Btree_load::free_pages_top(const Page_range_t &range) {
#ifdef UNIV_DEBUG
  mtr_t mtr;
  mtr_start(&mtr);
  page_t *root = btr_root_get(m_index, &mtr);
  fseg_header_t *seg_header = root + PAGE_HEADER + PAGE_BTR_SEG_TOP;
  ut_ad(memcmp(m_fseg_hdr_top, seg_header, FSEG_HEADER_SIZE) == 0);
  mtr_commit(&mtr);
#endif
  free_pages(range, m_fseg_hdr_top);
}

void Btree_load::free_pages(const Page_range_t &range,
                            fseg_header_t *fseg_hdr) {
  const space_id_t space_id = dict_index_get_space(m_index);

  if (range.first == range.second) {
    /* Nothing to do. */
    return;
  }

  ut_ad(range.first < range.second);
  Page_alloc_info info(dict_table_page_size(m_index->table));
  info.m_space_id = space_id;
  info.m_hint = FIL_NULL;
  info.m_fseg_header = fseg_hdr;

  mtr_t local_mtr;
  local_mtr.start();
  fseg_free_pages_of_extent(info, &local_mtr, range);
  local_mtr.commit();
}

void Btree_load::get_root_page_stat(Page_stat &stat) {
  const page_no_t subtree_root = get_subtree_root();
  ut_ad(subtree_root != FIL_NULL);
  mtr_t mtr;
  mtr.start();
  const space_id_t space_id = dict_index_get_space(m_index);
  const page_id_t page_id(space_id, subtree_root);
  const page_size_t page_size(dict_table_page_size(m_index->table));

  buf_block_t *block = btr_block_get(page_id, page_size, RW_S_LATCH,
                                     UT_LOCATION_HERE, m_index, &mtr);

  const auto page = buf_block_get_frame(block);
  stat.m_data_size = page_get_data_size(page);
  stat.m_n_recs = page_get_n_recs(page);
  mtr.commit();
}

void Page_load::free() {
  ut_ad(m_block->is_memory());
  buf_block_free(m_block);
  m_block = nullptr;
}

bool Blob_load::validate() {
  size_t not_full = 0;
  for (auto &idx_extent : m_index_extents) {
    if (!idx_extent->is_fully_used()) {
      not_full++;
    }
  }
  ut_ad(not_full == 0 || not_full == 1);
  return not_full == 0 || not_full == 1;
}

dberr_t Blob_load::add_index_extent() {
  const size_t leaf_level = 0;
  const bool is_leaf = true;
  const bool is_blob = true;
  Page_extent *idx_extent = Page_extent::create(m_btree_load, is_leaf, is_blob);
  m_index_extents.push_back(idx_extent);
  dberr_t err = m_btree_load->alloc_extent(idx_extent->m_range, leaf_level);
  if (err != DB_SUCCESS) {
    return err;
  }
  idx_extent->init();
  ut_ad(validate());
  return DB_SUCCESS;
}

buf_block_t *Blob_load::alloc_first_page() { return alloc_index_page(); }

buf_block_t *Blob_load::alloc_index_page() {
  const size_t leaf_level = 0;
  const bool is_leaf = true;

  if (m_index_extents.empty()) {
    dberr_t err = add_index_extent();
    if (err != DB_SUCCESS) {
      return nullptr;
    }
  }

  Page_extent *idx_extent = get_index_extent();
  if (idx_extent->is_fully_used()) {
    ut_ad(idx_extent->m_page_loads.size() > 0);
    const bool is_blob = true;
    Page_extent *extent = Page_extent::create(m_btree_load, is_leaf, is_blob);
    m_index_extents.push_back(extent);
    dberr_t err = m_btree_load->alloc_extent(extent->m_range, leaf_level);
    if (err != DB_SUCCESS) {
      return nullptr;
    }
    extent->init();
    idx_extent = get_index_extent();
    ut_ad(validate());
  }
  const page_no_t page_no = idx_extent->alloc();
  Page_load *page_load = Page_load::create(m_btree_load, idx_extent);
  page_load->init_blob(page_no);
  idx_extent->append(page_load);
  buf_block_t *block = page_load->get_block();
  m_btree_load->blob()->block_put(block);

#ifdef UNIV_DEBUG
  m_page_count++;
#endif /* UNIV_DEBUG */
  return block;
}

buf_block_t *Blob_load::alloc_data_page() {
  const size_t leaf_level = 0;
  const bool is_blob = true;
  const bool is_leaf = true;
  Page_extent *cur_extent{};

  /* Find not-fully-used extent. */
  for (auto iter = m_data_extents.begin(); iter != m_data_extents.end();) {
    auto data_extent = *iter;
    if (data_extent->is_page_loads_full()) {
      if (!m_btree_load->is_compressed()) {
        m_btree_load->add_to_bulk_flusher(data_extent);
        iter = m_data_extents.erase(iter);
      } else {
        ++iter;
      }
    } else {
      cur_extent = data_extent;
      break;
    }
  }

  ut_ad(cur_extent == nullptr || !cur_extent->is_page_loads_full());

  if (m_data_extents.empty() || cur_extent == nullptr) {
    cur_extent = Page_extent::create(m_btree_load, is_leaf, is_blob);
    dberr_t err = m_btree_load->alloc_extent(cur_extent->m_range, leaf_level);
    if (err != DB_SUCCESS) {
      return nullptr;
    }
    cur_extent->init();
    m_data_extents.push_back(cur_extent);
  }

  const page_no_t page_no = cur_extent->alloc();
  Page_load *page_load = Page_load::create(m_btree_load, cur_extent);
  page_load->init_blob(page_no);
  cur_extent->append(page_load);
#ifdef UNIV_DEBUG
  m_page_count++;
#endif /* UNIV_DEBUG */
  return page_load->get_block();
}

dberr_t Blob_load::finish() {
  while (!m_index_extents.empty()) {
    auto extent = m_index_extents.front();
    m_index_extents.pop_front();
    m_btree_load->track_extent(extent);
  }
  flush_data_extents();
  return DB_SUCCESS;
}

dberr_t Blob_load::flush_data_extents() {
  while (!m_data_extents.empty()) {
    auto extent = m_data_extents.front();
    m_data_extents.pop_front();
    m_btree_load->add_to_bulk_flusher(extent);
  }
  return DB_SUCCESS;
}

dberr_t Blob_load::flush_index_extents() {
#ifdef UNIV_DEBUG
  size_t n_not_full = 0;
#endif /* UNIV_DEBUG */
  while (!m_index_extents.empty()) {
    auto extent = m_index_extents.front();
    m_index_extents.pop_front();
#ifdef UNIV_DEBUG
    if (!extent->is_page_loads_full()) {
      ++n_not_full;
    }
#endif /* UNIV_DEBUG */
    m_btree_load->add_to_bulk_flusher(extent);
  }
  ut_ad(n_not_full <= 1);
  return DB_SUCCESS;
}

#ifdef UNIV_DEBUG
std::ostream &Blob_load::print_stats(std::ostream &out) const {
  out << "m_page_count=" << m_page_count << std::endl;
  return out;
}
#endif /* UNIV_DEBUG */

void Btree_load::track_extent(Page_extent *page_extent) {
  for (auto iter : m_extents_tracked) {
    ut_ad(iter != page_extent);
    if (page_extent == iter) {
      /* Ignore if already registered. */
      return;
    }
  }
  m_extents_tracked.push_back(page_extent);
}

bool Btree_load_compare::operator()(const Btree_load *l_btree,
                                    const Btree_load *r_btree) {
  const space_id_t space_id = dict_index_get_space(m_index);
  const page_size_t page_size(dict_table_page_size(m_index->table));
  const size_t l_root_level = l_btree->get_root_level();
  const size_t r_root_level = r_btree->get_root_level();

  ut_ad(!r_btree->m_first_page_nos.empty());
  ut_ad(!l_btree->m_first_page_nos.empty());

  const page_no_t l_page_no = l_btree->m_last_page_nos[l_root_level];
  const page_no_t r_page_no = r_btree->m_first_page_nos[r_root_level];
  const page_id_t l_page_id(space_id, l_page_no);
  const page_id_t r_page_id(space_id, r_page_no);
  Scoped_heap local_heap(2048, UT_LOCATION_HERE);
  mtr_t local_mtr;
  local_mtr.start();
  buf_block_t *l_block = buf_page_get(l_page_id, page_size, RW_S_LATCH,
                                      UT_LOCATION_HERE, &local_mtr);
  buf_block_t *r_block = buf_page_get(r_page_id, page_size, RW_S_LATCH,
                                      UT_LOCATION_HERE, &local_mtr);

  byte *l_frame = buf_block_get_frame(l_block);
  byte *r_frame = buf_block_get_frame(r_block);

  rec_t *r_rec = page_rec_get_next(page_get_infimum_rec(r_frame));
  rec_t *l_rec = page_rec_get_next(page_get_infimum_rec(l_frame));

  auto heap = local_heap.get();

  ulint *l_offsets = rec_get_offsets(l_rec, m_index, nullptr, ULINT_UNDEFINED,
                                     UT_LOCATION_HERE, &heap);
  ulint *r_offsets = rec_get_offsets(r_rec, m_index, nullptr, ULINT_UNDEFINED,
                                     UT_LOCATION_HERE, &heap);

  const bool spatial_index_non_leaf = false;
  const bool cmp_btree_recs = false;
  ulint matched_fields{};
  int rec_order =
      cmp_rec_rec(l_rec, r_rec, l_offsets, r_offsets, m_index,
                  spatial_index_non_leaf, &matched_fields, cmp_btree_recs);
  local_mtr.commit();
  return (rec_order < 0);
}

dberr_t Btree_load::init() {
  ut_ad(m_blob_load == nullptr);
  m_blob_load = Blob_load::create(this);
  if (m_blob_load == nullptr) {
    return DB_OUT_OF_MEMORY;
  }

  m_heap_order = mem_heap_create(1024, UT_LOCATION_HERE);
  if (m_heap_order == nullptr) {
    return DB_OUT_OF_MEMORY;
  }
  return DB_SUCCESS;
}

void Bulk_flusher::info() {
  const size_t sleep_duration = s_sleep_duration.count();
  const size_t total_sleep = m_n_sleep * sleep_duration;
  ib::info(ER_IB_BULK_FLUSHER_INFO, m_n_sleep, sleep_duration, total_sleep,
           m_pages_flushed);
}

bool Btree_load::is_tpc_enabled() const {
  const space_id_t space_id = m_index->space;
  fil_space_t *space = fil_space_acquire(space_id);
  const bool is_tpc = space->is_compressed();
  const page_size_t page_size(space->flags);
  fil_space_release(space);
  return is_tpc && !page_size.is_compressed() &&
         IORequest::is_punch_hole_supported();
}

bool Btree_load::is_tpe_enabled() const {
  const space_id_t space_id = m_index->space;
  fil_space_t *space = fil_space_acquire(space_id);
  bool is_tpe = space->is_encrypted();
  fil_space_release(space);
  return is_tpe;
}

dberr_t Btree_load::Merger::merge(bool sort) {
  remove_empty_subtrees();

  /* All sub-trees were empty. Nothing to do. */
  if (m_btree_loads.empty()) {
    return DB_SUCCESS;
  }

  if (sort) {
    Btree_load_compare cmp_obj(m_index);
    std::sort(m_btree_loads.begin(), m_btree_loads.end(), cmp_obj);
  }

  for (size_t i = 0; i < m_btree_loads.size(); ++i) {
    size_t j = i + 1;
    if (j < m_btree_loads.size()) {
      Btree_load *l_btree = m_btree_loads[i];
      Btree_load *r_btree = m_btree_loads[j];
      if (l_btree->does_keys_overlap(r_btree)) {
        my_error(ER_LOAD_BULK_DATA_UNSORTED, MYF(0), l_btree->get_table_name(),
                 "");
        return DB_ERROR;
      }
    }
  }

  ut_d(validate_boundaries());

  size_t highest_level;
  dberr_t err = subtree_link_levels(highest_level);
  ut_ad(err == DB_SUCCESS);

  if (err == DB_SUCCESS) {
    err = add_root_for_subtrees(highest_level);
  }
  return err;
}

void Btree_load::Merger::remove_empty_subtrees() {
  for (auto iter = m_btree_loads.begin(); iter != m_btree_loads.end();) {
    auto btree_load = *iter;
    if (btree_load->m_first_page_nos.empty()) {
      ut::delete_(btree_load);
      iter = m_btree_loads.erase(iter);
    } else {
      ++iter;
    }
  }
}

#ifdef UNIV_DEBUG
void Btree_load::Merger::validate_boundaries() {
  for (auto btree_load : m_btree_loads) {
    ut_ad(!btree_load->m_first_page_nos.empty());
    ut_ad(!btree_load->m_last_page_nos.empty());
    ut_ad(btree_load->m_first_page_nos.size() ==
          btree_load->m_last_page_nos.size());
  }
}
#endif /* UNIV_DEBUG */

dberr_t Btree_load::Merger::subtree_link_levels(size_t &highest_level) {
  mtr_t *mtr;
  Scoped_heap local_heap(2048, UT_LOCATION_HERE);
  using stl_alloc_t = mem_heap_allocator<Btree_load *>;
  stl_alloc_t local_alloc(local_heap.get());
  auto mtr_heap = local_heap.alloc(sizeof(mtr_t));
  mtr = new (mtr_heap) mtr_t();
  highest_level = 0;
  const space_id_t space_id = dict_index_get_space(m_index);
  const page_size_t page_size(dict_table_page_size(m_index->table));
  using List = std::list<Btree_load *, stl_alloc_t>;
  auto from_list_raw = local_heap.alloc(sizeof(List));
  if (from_list_raw == nullptr) {
    return DB_OUT_OF_MEMORY;
  }
  List *from_list = new (from_list_raw) List(local_alloc);
  auto to_list_raw = local_heap.alloc(sizeof(List));
  if (to_list_raw == nullptr) {
    return DB_OUT_OF_MEMORY;
  }
  List *to_list = new (to_list_raw) List(local_alloc);
  /* Populate the from list.  Also calculate the highest level. */
  for (auto btree_load : m_btree_loads) {
    const size_t root_level = btree_load->get_root_level();
    const size_t tree_height = root_level + 1;

    ib::info(ER_IB_BULK_LOAD_SUBTREE_INFO, (size_t)space_id,
             m_index->table_name, m_index->name(), tree_height,
             btree_load->m_stat_n_extents, btree_load->m_stat_n_pages);

    if (root_level > highest_level) {
      highest_level = root_level;
    }
    from_list->push_back(btree_load);
#ifdef UNIV_DEBUG
    for (size_t cur_level = 0; cur_level < root_level; ++cur_level) {
      const page_no_t leftmost = btree_load->m_first_page_nos[cur_level];
      const page_no_t rightmost = btree_load->m_last_page_nos[cur_level];
      ut_ad(rightmost != leftmost);
    }
    {
      const page_no_t leftmost = btree_load->m_first_page_nos[root_level];
      const page_no_t rightmost = btree_load->m_last_page_nos[root_level];
      ut_ad(rightmost == leftmost);
    }
#endif /* UNIV_DEBUG */
  }

  /** Loop till all subtrees are at same level or only one subtree remaining.*/
  const size_t MAX_LOOP = from_list->size();

  for (size_t n_loop = 0;; ++n_loop) {
    if (n_loop >= MAX_LOOP) {
      std::ostringstream sout;
      sout << "{From list size: " << from_list->size() << ", {";
      for (auto &b_load : *from_list) {
        sout << b_load->get_root_level() << ", ";
      }
      sout << "}}";
      LogErr(ERROR_LEVEL, ER_IB_BULK_LOAD_MERGE_FAIL,
             "Btree_load::Merger::subtree_link_levels()", m_index->table_name,
             m_index->name(), sout.str().c_str());
      ut_d(const bool bulk_load_merge_failed = false);
      ut_ad(bulk_load_merge_failed);
      return DB_FAIL;
    }

    /* There is only one subtree.  */
    if (from_list->size() == 1) {
      break;
    }

    while (!from_list->empty()) {
      Btree_load *subtree_1 = from_list->front();
      from_list->pop_front();
      if (from_list->empty()) {
        to_list->push_back(subtree_1);
        break;
      }
      Btree_load *subtree_2 = from_list->front();
      from_list->pop_front();

      /* All keys in subtree_1 must be less than all keys in subtree_2 */
      const size_t level_1 = subtree_1->get_root_level();
      const size_t level_2 = subtree_2->get_root_level();
      const size_t level = std::min(level_1, level_2);

      for (size_t cur_level = 0; cur_level <= level; cur_level++) {
        const page_no_t l_page_no = subtree_1->m_last_page_nos[cur_level];
        const page_no_t r_page_no = subtree_2->m_first_page_nos[cur_level];

        const page_id_t l_page_id(space_id, l_page_no);
        const page_id_t r_page_id(space_id, r_page_no);

        mtr->start();
        buf_block_t *l_block = buf_page_get(l_page_id, page_size, RW_X_LATCH,
                                            UT_LOCATION_HERE, mtr);
        buf_block_t *r_block = buf_page_get(r_page_id, page_size, RW_X_LATCH,
                                            UT_LOCATION_HERE, mtr);

#ifdef UNIV_DEBUG
        const page_type_t l_type = l_block->get_page_type();
        const page_type_t r_type = r_block->get_page_type();
        ut_a(l_type == FIL_PAGE_INDEX);
        ut_a(r_type == FIL_PAGE_INDEX);
#endif /* UNIV_DEBUG */

        byte *l_frame = buf_block_get_frame(l_block);
        byte *r_frame = buf_block_get_frame(r_block);

        page_zip_des_t *l_zip = buf_block_get_page_zip(l_block);
        page_zip_des_t *r_zip = buf_block_get_page_zip(r_block);

        btr_page_set_next(l_frame, l_zip, r_page_no, mtr);
        btr_page_set_prev(r_frame, r_zip, l_page_no, mtr);

        rec_t *first_rec = page_rec_get_next(page_get_infimum_rec(r_frame));

        btr_unset_min_rec_mark(r_block, first_rec, mtr);

#ifdef UNIV_DEBUG
        {
          rec_t *l_rec = page_rec_get_prev(page_get_supremum_rec(l_frame));
          rec_t *r_rec = first_rec;
          auto heap = local_heap.get();

          ulint *l_offsets =
              rec_get_offsets(l_rec, m_index, nullptr, ULINT_UNDEFINED,
                              UT_LOCATION_HERE, &heap);
          ulint *r_offsets =
              rec_get_offsets(r_rec, m_index, nullptr, ULINT_UNDEFINED,
                              UT_LOCATION_HERE, &heap);

          const bool spatial_index_non_leaf = false;
          const bool cmp_btree_recs = false;
          ulint matched_fields{};
          int rec_order = cmp_rec_rec(l_rec, r_rec, l_offsets, r_offsets,
                                      m_index, spatial_index_non_leaf,
                                      &matched_fields, cmp_btree_recs);
          ut_ad(rec_order <= 0);
        }
#endif /* UNIV_DEBUG */

        mtr->commit();
      }
      if (level_1 == level_2) {
        to_list->push_back(subtree_1);
        from_list->push_front(subtree_2);
      } else if (level_1 < level_2) {
        const page_no_t l_page_no = subtree_1->m_last_page_nos[level_1];
        const page_no_t r_page_no = subtree_2->m_first_page_nos[level_1 + 1];

        const page_id_t l_page_id(space_id, l_page_no);
        const page_id_t r_page_id(space_id, r_page_no);

        /* Load the two pages. */
        mtr->start();
        buf_block_t *l_block = buf_page_get(l_page_id, page_size, RW_X_LATCH,
                                            UT_LOCATION_HERE, mtr);
        buf_block_t *r_block = buf_page_get(r_page_id, page_size, RW_X_LATCH,
                                            UT_LOCATION_HERE, mtr);

        byte *l_frame = buf_block_get_frame(l_block);
        byte *r_frame = buf_block_get_frame(r_block);

        auto r_first_rec = page_rec_get_next(page_get_infimum_rec(r_frame));
        btr_unset_min_rec_mark(r_block, r_first_rec, mtr);

        /* Obtain node ptr of left page. */
        auto l_first_rec = page_rec_get_next(page_get_infimum_rec(l_frame));
        ut_a(page_rec_is_user_rec(l_first_rec));
        auto node_ptr = dict_index_build_node_ptr(
            m_index, l_first_rec, l_page_no, local_heap.get(), level_1);

        /* Insert node ptr into higher right page. */
        page_cur_t page_cur;
        page_cur_set_before_first(r_block, &page_cur);

        ulint *offsets{};
        mem_heap_t *heap = local_heap.get();
        rec_t *insert_rec = page_cur_tuple_insert(&page_cur, node_ptr, m_index,
                                                  &offsets, &heap, mtr);
        ut_ad(insert_rec != nullptr);
#ifdef UNIV_DEBUG
        {
          rec_t *next_rec = page_rec_get_next(insert_rec);
          const page_no_t right_page_no = btr_page_get_next(l_frame, mtr);
          ulint *node_ptr_offsets =
              rec_get_offsets(next_rec, m_index, nullptr, ULINT_UNDEFINED,
                              UT_LOCATION_HERE, &heap);
          const page_no_t right_child_no =
              btr_node_ptr_get_child_page_no(next_rec, node_ptr_offsets);
          ut_ad(right_page_no == right_child_no);
        }
#endif /* UNIV_DEBUG */
        btr_set_min_rec_mark(insert_rec, mtr);
        mtr->commit();
        from_list->push_front(subtree_2);

        for (size_t cur_level = 0; cur_level <= level_1; cur_level++) {
          subtree_2->m_first_page_nos[cur_level] =
              subtree_1->m_first_page_nos[cur_level];
        }
        ut::delete_(subtree_1);
      } else if (level_1 > level_2) {
        /* Left subtree is taller. */
        const page_no_t l_page_no = subtree_1->m_last_page_nos[level_2 + 1];
        const page_no_t r_page_no = subtree_2->m_first_page_nos[level_2];

        const page_id_t l_page_id(space_id, l_page_no);
        const page_id_t r_page_id(space_id, r_page_no);

        /* Load the two pages. */
        mtr->start();
        buf_block_t *l_block = buf_page_get(l_page_id, page_size, RW_X_LATCH,
                                            UT_LOCATION_HERE, mtr);
        buf_block_t *r_block = buf_page_get(r_page_id, page_size, RW_X_LATCH,
                                            UT_LOCATION_HERE, mtr);

        byte *r_frame = buf_block_get_frame(r_block);

        /* Obtain node ptr of right page. */
        auto r_first_rec = page_rec_get_next(page_get_infimum_rec(r_frame));
        ut_a(page_rec_is_user_rec(r_first_rec));
        btr_unset_min_rec_mark(r_block, r_first_rec, mtr);

        auto node_ptr = dict_index_build_node_ptr(
            m_index, r_first_rec, r_page_no, local_heap.get(), level_2);

        /* Insert node ptr into higher left page. */
        page_cur_t page_cur;
        page_cur_search(l_block, m_index, node_ptr, &page_cur);

        ulint *offsets{};
        mem_heap_t *heap = local_heap.get();
        rec_t *inserted = page_cur_tuple_insert(&page_cur, node_ptr, m_index,
                                                &offsets, &heap, mtr);
        ut_a(inserted != nullptr);

        mtr->commit();
        from_list->push_front(subtree_1);
        for (size_t cur_level = 0; cur_level <= level_2; cur_level++) {
          subtree_1->m_last_page_nos[cur_level] =
              subtree_2->m_last_page_nos[cur_level];
        }
        ut::delete_(subtree_2);
      }
    }
    std::swap(from_list, to_list);

    /* Check if all subtrees are same level. */
    const bool same_level =
        std::all_of(from_list->begin(), from_list->end(),
                    [highest_level](Btree_load *load) {
                      return load->get_root_level() == highest_level;
                    });

    if (same_level) {
      ut_ad(std::is_sorted(from_list->begin(), from_list->end(),
                           Btree_load_compare(m_index)));
      break;
    }
  }

  m_btree_loads.clear();
  while (!from_list->empty()) {
    Btree_load *subtree = from_list->front();
    from_list->pop_front();
    m_btree_loads.push_back(subtree);
  }
  return DB_SUCCESS;
}

dberr_t Btree_load::Merger::add_root_for_subtrees(const size_t highest_level) {
  /* This function uses mtr with MTR_LOG_NO_REDO and a flush observer. */
  if (m_btree_loads.empty()) {
    return DB_SUCCESS;
  }
  ut_ad(std::is_sorted(m_btree_loads.begin(), m_btree_loads.end(),
                       Btree_load_compare(m_index)));
  Scoped_heap tuple_heap(2048, UT_LOCATION_HERE);

  const page_no_t root_page_no = dict_index_get_page(m_index);
  auto observer = m_trx->flush_observer;

  size_t n_subtrees = 0;
  size_t n_root_data = 0;
  size_t n_root_recs = 0;
  for (size_t i = 0; i < m_btree_loads.size(); ++i) {
    const auto level = m_btree_loads[i]->get_root_level();
    if (level == highest_level) {
      n_subtrees++;
      Page_stat page_stat;
      m_btree_loads[i]->get_root_page_stat(page_stat);
      n_root_data += page_stat.m_data_size;
      n_root_recs += page_stat.m_n_recs;
    }
  }

  const size_t slot_size = page_dir_calc_reserved_space(n_root_recs);
  const size_t need_space = n_root_data + slot_size;
  const size_t max_free = get_max_free();
  const bool level_incr = (n_subtrees > 1) && (need_space >= max_free);
  const size_t new_root_level = level_incr ? highest_level + 1 : highest_level;

  Page_load root_load(m_index, m_trx->id, root_page_no, new_root_level,
                      observer);
  mtr_t mtr;
  mtr.start();
  mtr.x_lock(dict_index_get_lock(m_index), UT_LOCATION_HERE);

  auto err = root_load.init();
  if (err != DB_SUCCESS) {
    mtr.commit();
    return err;
  }

  bool min_rec = true;
  for (size_t i = 0; i < m_btree_loads.size(); ++i) {
    const page_no_t subtree_root = m_btree_loads[i]->get_subtree_root();
    const size_t tree_level = m_btree_loads[i]->get_root_level();

    if (tree_level != highest_level) {
      /* Skip smaller sub-trees.  */
      continue;
    }

    const page_id_t page_id(dict_index_get_space(m_index), subtree_root);
    page_size_t page_size(dict_table_page_size(m_index->table));

    buf_block_t *subtree_block = btr_block_get(page_id, page_size, RW_X_LATCH,
                                               UT_LOCATION_HERE, m_index, &mtr);
    auto subtree_page = buf_block_get_frame(subtree_block);
    auto first_rec = page_rec_get_next(page_get_infimum_rec(subtree_page));
    ut_a(page_rec_is_user_rec(first_rec));

    if (highest_level > 0) {
      min_rec ? btr_set_min_rec_mark(first_rec, &mtr)
              : btr_unset_min_rec_mark(subtree_block, first_rec, &mtr);
    }

    if (level_incr) {
      auto node_ptr = dict_index_build_node_ptr(
          m_index, first_rec, subtree_root, tuple_heap.get(), highest_level);
      auto rec_size = rec_get_converted_size(m_index, node_ptr);

      if (min_rec) {
        node_ptr->set_min_rec_flag();
      }

      err = root_load.insert(node_ptr, nullptr, rec_size);
      if (err != DB_SUCCESS) {
        return err;
      }
    } else {
      /* Copy the records from subtree root to actual root. */
      (void)root_load.copy_all(subtree_page);

      /* Remove the subtree root. */
      btr_page_free_low(m_index, subtree_block, highest_level, &mtr);
    }
    min_rec = false;
  }
  root_load.set_next(FIL_NULL);
  root_load.set_prev(FIL_NULL);
  root_load.finish();
  mtr.commit();
  root_load.commit();
  if (root_load.is_table_compressed()) {
    err = root_page_commit(&root_load);
  }
  return err;
}

bool Btree_load::does_keys_overlap(const Btree_load *r_btree) const {
  const Btree_load *l_btree = this;
  const space_id_t space_id = dict_index_get_space(m_index);
  const page_size_t page_size(dict_table_page_size(m_index->table));

  ut_ad(!r_btree->m_first_page_nos.empty());
  ut_ad(!l_btree->m_first_page_nos.empty());

  const page_no_t l_page_no = l_btree->m_last_page_nos[0];
  const page_no_t r_page_no = r_btree->m_first_page_nos[0];
  const page_id_t l_page_id(space_id, l_page_no);
  const page_id_t r_page_id(space_id, r_page_no);
  Scoped_heap local_heap(2048, UT_LOCATION_HERE);
  mtr_t local_mtr;
  local_mtr.start();
  buf_block_t *l_block = buf_page_get(l_page_id, page_size, RW_S_LATCH,
                                      UT_LOCATION_HERE, &local_mtr);
  buf_block_t *r_block = buf_page_get(r_page_id, page_size, RW_S_LATCH,
                                      UT_LOCATION_HERE, &local_mtr);

  byte *l_frame = buf_block_get_frame(l_block);
  byte *r_frame = buf_block_get_frame(r_block);

  rec_t *r_rec = page_rec_get_next(page_get_infimum_rec(r_frame));
  rec_t *l_rec = page_rec_get_prev(page_get_supremum_rec(l_frame));

  auto heap = local_heap.get();

  ulint *l_offsets = rec_get_offsets(l_rec, m_index, nullptr, ULINT_UNDEFINED,
                                     UT_LOCATION_HERE, &heap);
  ulint *r_offsets = rec_get_offsets(r_rec, m_index, nullptr, ULINT_UNDEFINED,
                                     UT_LOCATION_HERE, &heap);

  const bool spatial_index_non_leaf = false;
  const bool cmp_btree_recs = false;
  ulint matched_fields{};
  int rec_order =
      cmp_rec_rec(l_rec, r_rec, l_offsets, r_offsets, m_index,
                  spatial_index_non_leaf, &matched_fields, cmp_btree_recs);
  local_mtr.commit();
  return (rec_order >= 0);
}

Level_ctx::~Level_ctx() {}

Page_load::~Page_load() noexcept {
  if (m_heap != nullptr) {
    /* mtr is allocated using heap. */
    if (m_mtr != nullptr) {
      ut_a(!m_mtr->is_active());
      m_mtr->~mtr_t();
    }
    mem_heap_free(m_heap);
  }
}

dberr_t Btree_load::Merger::root_split(Page_load *root_load,
                                       const size_t n_pages) {
  const size_t root_level = root_load->get_level();
  Flush_observer *observer = m_trx->flush_observer;
  const auto trx_id = m_trx->id;
  dberr_t err{DB_SUCCESS};

  /* Load the given root page. */
  root_load->latch();

  /* Allocate requested number of pages in the given level. */
  std::vector<Page_load *> page_loads;

  auto guard = create_scope_guard([root_load, &page_loads]() {
    root_load->finish();
    root_load->commit();

    for (auto &page_load : page_loads) {
      ut::delete_(page_load);
    }
  });

  for (size_t i = 0; i < n_pages; ++i) {
    auto page_load =
        ut::new_withkey<Page_load>(UT_NEW_THIS_FILE_PSI_KEY, m_index, trx_id,
                                   FIL_NULL, root_level, observer, nullptr);
    if (page_load == nullptr) {
      return DB_OUT_OF_MEMORY;
    }
    page_loads.push_back(page_load);
    err = page_load->alloc();
    if (err != DB_SUCCESS) {
      return err;
    }
    err = page_load->init();
    if (err != DB_SUCCESS) {
      return err;
    }
  }

  /* Set the FIL_PAGE_PREV and FIL_PAGE_NEXT for these new pages. */
  for (size_t i = 0; i < n_pages; ++i) {
    page_no_t p = FIL_NULL;
    page_no_t n = FIL_NULL;
    if (i == 0) {
      n = page_loads[i + 1]->get_page_no();
    } else if (i == (n_pages - 1)) {
      p = page_loads[i - 1]->get_page_no();
    } else {
      p = page_loads[i - 1]->get_page_no();
      n = page_loads[i + 1]->get_page_no();
    }
    page_loads[i]->set_prev(p);
    page_loads[i]->set_next(n);
  }

  /* Copy records from given root pages to the new pages. */
  root_load->copy_to(page_loads);

  // Compress each of these N pages.
  for (size_t i = 0; i < n_pages; ++i) {
    page_loads[i]->finish();
    page_loads[i]->commit();
    bool success = page_loads[i]->compress();
    if (!success) {
      return DB_FAIL;
    }
  }

  /* Empty the root page. */
  root_load->set_level(root_level + 1);
  err = root_load->reinit();
  ut_a(err == DB_SUCCESS);

  /* Insert N node_ptrs into root page. */
  for (size_t i = 0; i < n_pages; ++i) {
    dtuple_t *node_ptr = page_loads[i]->get_node_ptr();
    big_rec_t *big_rec{};
    auto rec_size = rec_get_converted_size(m_index, node_ptr);
    err = root_load->insert(node_ptr, big_rec, rec_size);
    ut_a(err == DB_SUCCESS);
  }

  guard.commit();
  root_load->finish();
  root_load->commit();

  /* Compress the root page. */
  bool success = root_load->compress();
  if (!success) {
    return DB_FAIL;
  }

  return DB_SUCCESS;
}

dberr_t Btree_load::Merger::root_page_commit(Page_load *root_load) {
  ut_a(root_load);
  root_load->set_next(FIL_NULL);
  root_load->set_prev(FIL_NULL);
  dberr_t err{DB_SUCCESS};

  if (root_load->is_table_compressed() && !root_load->compress()) {
    /* We split the root page into 2 and compress, if that fails we split the
    root page into 3 and compress and so on upto a maximum of 6.  */
    const size_t max_split = 6;
    for (size_t i = 2; i < max_split; ++i) {
      err = root_split(root_load, i);
      if (err == DB_SUCCESS) {
        break;
      }
    }
  }
  return err;
}
