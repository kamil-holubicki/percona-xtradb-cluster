/******************************************************
INFORMATION SCHEMA innodb_trx, innodb_locks and
innodb_lock_waits tables fetch code.

The code below fetches information needed to fill those
3 dynamic tables and uploads it into a "transactions
table cache" for later retrieval.

(c) 2007 Innobase Oy

Created July 17, 2007 Vasil Dimov
*******************************************************/

#include <mysql/plugin.h>

#include "univ.i"
#include "hash0hash.h"
#include "lock0iter.h"
#include "lock0lock.h"
#include "mem0mem.h"
#include "srv0srv.h"
#include "sync0rw.h"
#include "sync0sync.h"
#include "sync0types.h"
#include "trx0i_s.h"
#include "trx0sys.h"
#include "trx0trx.h"
#include "ut0mem.h"
#include "ut0ut.h"

#define TABLE_CACHE_INITIAL_ROWSNUM	1024

/* Table cache's rows are stored in a set of chunks. When a new row is
added a new chunk is allocated if necessary. MEM_CHUNKS_IN_TABLE_CACHE
specifies the maximum number of chunks.
Assuming that the first one is 1024 rows (TABLE_CACHE_INITIAL_ROWSNUM)
and each subsequent is N/2 where N is the number of rows we have
allocated till now, then 39th chunk would have 1677416425 number of rows
and all chunks would have 3354832851 number of rows. */
#define MEM_CHUNKS_IN_TABLE_CACHE	39

/* XXX delete this macro as soon as a real thd_get_thread_id()
function is added to the MySQL code, bug
http://bugs.mysql.com/30930 has been opened for this */
#define thd_get_thread_id(thd)		0

/* The following are some testing auxiliary macros. Do not enable them
in a production environment. */

#if 0
/* If this is enabled then lock folds will always be different
resulting in equal rows being put in a different cells of the hash
table. Checking for duplicates will be flawed because different
fold will be calculated when a row is searched in the hash table. */
#define TEST_LOCK_FOLD_ALWAYS_DIFFERENT
#endif

#if 0
/* This effectively kills the searching for duplicate before adding a
row function. It will always be assumed that lock is not present and
insertion will be performed in the hash table. */
#define TEST_NO_LOCKS_ROW_IS_EVER_EQUAL_TO_LOCK_T
#endif

#if 0
/* This aggressively repeats adding each row many times. Depending on
the above settings this may be noop or may result in lots of rows being
added. */
#define TEST_ADD_EACH_LOCKS_ROW_MANY_TIMES
#endif

#if 0
/* Very similar to TEST_NO_LOCKS_ROW_IS_EVER_EQUAL_TO_LOCK_T but hash
table search is not performed at all. */
#define TEST_DO_NOT_CHECK_FOR_DUPLICATE_ROWS
#endif

#if 0
/* Do not insert each row into the hash table, duplicates may appear
if this is enabled, also if this is enabled searching into the hash is
noop because it will be empty. */
#define TEST_DO_NOT_INSERT_INTO_THE_HASH_TABLE
#endif

/* Memory for each table in the intermediate buffer is allocated in
separate chunks. These chunks are considered to be concatenated to
represent one flat array of rows. */
typedef struct i_s_mem_chunk_struct {
	ulint	offset;		/* offset, in number of rows */
	ulint	rows_allocd;	/* the size of this chunk, in number
				of rows */
	void*	base;		/* start of the chunk */
} i_s_mem_chunk_t;

/* This represents one table's cache. */
typedef struct i_s_table_cache_struct {
	ullint		rows_used;	/* number of used rows */
	ullint		rows_allocd;	/* number of allocated rows */
	ulint		row_size;	/* size of a single row */
	i_s_mem_chunk_t	chunks[MEM_CHUNKS_IN_TABLE_CACHE]; /* array of
					memory chunks that stores the
					rows */
} i_s_table_cache_t;

/* This structure describes the intermediate buffer */
struct trx_i_s_cache_struct {
	rw_lock_t	rw_lock;	/* read-write lock protecting
					the rest of this structure */
	ullint		last_read;	/* last time the cache was read;
					measured in microseconds since
					epoch */
	mutex_t		last_read_mutex;/* mutex protecting the
					last_read member - it is updated
					inside a shared lock of the
					rw_lock member */
	i_s_table_cache_t innodb_trx;	/* innodb_trx table */
	i_s_table_cache_t innodb_locks;	/* innodb_locks table */
	i_s_table_cache_t innodb_lock_waits;/* innodb_lock_waits table */
/* the hash table size is LOCKS_HASH_CELLS_NUM * sizeof(void*) bytes */
#define LOCKS_HASH_CELLS_NUM	10000
	hash_table_t*	locks_hash;	/* hash table used to eliminate
					duplicate entries in the
					innodb_locks table */
};

/* This is the intermediate buffer where data needed to fill the
INFORMATION SCHEMA tables is fetched and later retrieved by the C++
code in handler/i_s.cc. */
static trx_i_s_cache_t	trx_i_s_cache_static;
trx_i_s_cache_t*	trx_i_s_cache = &trx_i_s_cache_static;

/***********************************************************************
For a record lock that is in waiting state retrieves the only bit that
is set, for a table lock returns ULINT_UNDEFINED. */
static
ulint
wait_lock_get_heap_no(
/*==================*/
				/* out: record number within the heap */
	const lock_t*	lock)	/* in: lock */
{
	ulint	ret;

	switch (lock_get_type(lock)) {
	case LOCK_REC:
		ret = lock_rec_find_set_bit(lock);
		ut_a(ret != ULINT_UNDEFINED);
		break;
	case LOCK_TABLE:
		ret = ULINT_UNDEFINED;
		break;
	default:
		ut_error;
	}

	return(ret);
}

/***********************************************************************
Initializes the members of a table cache. */
static
void
table_cache_init(
/*=============*/
	i_s_table_cache_t*	table_cache,	/* out: table cache */
	size_t			row_size)	/* in: the size of a
						row */
{
	ulint	i;

	table_cache->rows_used = 0;
	table_cache->rows_allocd = 0;
	table_cache->row_size = row_size;

	for (i = 0; i < MEM_CHUNKS_IN_TABLE_CACHE; i++) {

		/* the memory is actually allocated in
		table_cache_create_empty_row() */
		table_cache->chunks[i].base = NULL;
	}
}

/***********************************************************************
Returns an empty row from a table cache. The row is allocated if no more
empty rows are available. The number of used rows is incremented. */
static
void*
table_cache_create_empty_row(
/*=========================*/
						/* out: empty row */
	i_s_table_cache_t*	table_cache)	/* in/out: table cache */
{
	ulint	i;
	void*	row;

	ut_a(table_cache->rows_used <= table_cache->rows_allocd);

	if (table_cache->rows_used == table_cache->rows_allocd) {

		/* rows_used == rows_allocd means that new chunk needs
		to be allocated: either no more empty rows in the
		last allocated chunk or nothing has been allocated yet
		(rows_num == rows_allocd == 0); */

		i_s_mem_chunk_t*	chunk;
		ulint			req_bytes;
		ulint			got_bytes;
		ulint			req_rows;
		ulint			got_rows;

		/* find the first not allocated chunk */
		for (i = 0; i < MEM_CHUNKS_IN_TABLE_CACHE; i++) {

			if (table_cache->chunks[i].base == NULL) {

				break;
			}
		}

		/* i == MEM_CHUNKS_IN_TABLE_CACHE means that all chunks
		have been allocated :-X */
		ut_a(i < MEM_CHUNKS_IN_TABLE_CACHE);

		/* allocate the chunk we just found */

		if (i == 0) {

			/* first chunk, nothing is allocated yet */
			req_rows = TABLE_CACHE_INITIAL_ROWSNUM;
		} else {

			/* Memory is increased by the formula
			new = old + old / 2; We are trying not to be
			aggressive here (= using the common new = old * 2)
			because the allocated memory will not be freed
			until InnoDB exit (it is reused). So it is better
			to once allocate the memory in more steps, but
			have less unused/wasted memory than to use less
			steps in allocation (which is done once in a
			lifetime) but end up with lots of unused/wasted
			memory. */
			req_rows = table_cache->rows_allocd / 2;
		}
		req_bytes = req_rows * table_cache->row_size;

		chunk = &table_cache->chunks[i];

		chunk->base = mem_alloc2(req_bytes, &got_bytes);

		got_rows = got_bytes / table_cache->row_size;

#if 0
		printf("allocating chunk %d req bytes=%lu, got bytes=%lu, "
		       "row size=%lu, "
		       "req rows=%lu, got rows=%lu\n",
		       i, req_bytes, got_bytes,
		       table_cache->row_size,
		       req_rows, got_rows);
#endif

		chunk->rows_allocd = got_rows;

		table_cache->rows_allocd += got_rows;

		/* adjust the offset of the next chunk */
		if (i < MEM_CHUNKS_IN_TABLE_CACHE - 1) {

			table_cache->chunks[i + 1].offset
				= chunk->offset + chunk->rows_allocd;
		}

		/* return the first empty row in the newly allocated
		chunk */
		row = chunk->base;
	} else {

		char*	chunk_start;
		ulint	offset;

		/* there is an empty row, no need to allocate new
		chunks */

		/* find the first chunk that contains allocated but
		empty/unused rows */
		for (i = 0; i < MEM_CHUNKS_IN_TABLE_CACHE; i++) {

			if (table_cache->chunks[i].offset
			    + table_cache->chunks[i].rows_allocd
			    > table_cache->rows_used) {

				break;
			}
		}

		/* i == MEM_CHUNKS_IN_TABLE_CACHE means that all chunks
		are full, but
		table_cache->rows_used != table_cache->rows_allocd means
		exactly the opposite - there are allocated but
		empty/unused rows :-X */
		ut_a(i < MEM_CHUNKS_IN_TABLE_CACHE);

		chunk_start = (char*) table_cache->chunks[i].base;
		offset = table_cache->rows_used
			- table_cache->chunks[i].offset;

		row = chunk_start + offset * table_cache->row_size;
	}

	table_cache->rows_used++;

	return(row);
}

/***********************************************************************
Fills i_s_trx_row_t object. Returns its first argument. */
static
i_s_trx_row_t*
fill_trx_row(
/*=========*/
						/* out: result object
						that's filled */
	i_s_trx_row_t*		row,		/* out: result object
						that's filled */
	const trx_t*		trx,		/* in: transaction to
						get data from */
	const i_s_locks_row_t*	wait_lock_row)	/* in: pointer to the
						corresponding row in
						innodb_locks if trx is
						waiting or NULL if trx
						is not waiting */
{
	row->trx_id = trx_get_id(trx);
	row->trx_started = (ib_time_t) trx->start_time;
	row->trx_state = trx_get_que_state_str(trx);

	if (trx->wait_lock != NULL) {

		ut_a(wait_lock_row != NULL);

		row->wait_lock_row = wait_lock_row;
		row->trx_wait_started = (ib_time_t) trx->wait_started;
	} else {

		ut_a(wait_lock_row == NULL);

		row->wait_lock_row = NULL;
		row->trx_wait_started = 0;
	}

	row->trx_mysql_thread_id = thd_get_thread_id(trx->mysql_thd);

	return(row);
}

/***********************************************************************
Fills i_s_locks_row_t object. Returns its first argument. */
static
i_s_locks_row_t*
fill_locks_row(
/*===========*/
				/* out: result object that's filled */
	i_s_locks_row_t* row,	/* out: result object that's filled */
	const lock_t*	lock,	/* in: lock to get data from */
	ulint		heap_no)/* in: lock's record number
				or ULINT_UNDEFINED if the lock
				is a table lock */
{
	row->lock_trx_id = lock_get_trx_id(lock);
	row->lock_mode = lock_get_mode_str(lock);
	row->lock_type = lock_get_type_str(lock);

	/* XXX this may be freed later */
	row->lock_table = lock_get_table_name(lock);

	switch (lock_get_type(lock)) {
	case LOCK_REC:
		/* XXX this may be freed later */
		row->lock_index = lock_rec_get_index_name(lock);

		row->lock_space = lock_rec_get_space_id(lock);
		row->lock_page = lock_rec_get_page_no(lock);
		row->lock_rec = heap_no;

		break;
	case LOCK_TABLE:
		row->lock_index = NULL;

		row->lock_space = ULINT_UNDEFINED;
		row->lock_page = ULINT_UNDEFINED;
		row->lock_rec = ULINT_UNDEFINED;

		break;
	default:
		ut_error;
	}

	row->lock_table_id = lock_get_table_id(lock);

	row->hash_chain.value = row;

	return(row);
}

/***********************************************************************
Fills i_s_lock_waits_row_t object. Returns its first argument. */
static
i_s_lock_waits_row_t*
fill_lock_waits_row(
/*================*/
						/* out: result object
						that's filled */
	i_s_lock_waits_row_t*	row,		/* out: result object
						that's filled */
	const i_s_locks_row_t*	wait_lock_row,	/* in: pointer to the
						relevant wait-lock
						row in innodb_locks */
	const i_s_locks_row_t*	waited_lock_row)/* in: pointer to the
						relevant waited-lock
						row in innodb_locks */
{
	row->wait_lock_row = wait_lock_row;
	row->waited_lock_row = waited_lock_row;

	return(row);
}

/***********************************************************************
Calculates a hash fold for a lock. For a record lock the fold is
calculated from 4 elements, which uniquely identify a lock at a given
point in time: transaction id, space id, page number, record number.
For a table lock the fold is table's id. */
static
ulint
fold_lock(
/*======*/
				/* out: fold */
	const lock_t*	lock,	/* in: lock object to fold */
	ulint		heap_no)/* in: lock's record number
				or ULINT_UNDEFINED if the lock
				is a table lock */
{
#ifdef TEST_LOCK_FOLD_ALWAYS_DIFFERENT
	static ulint	fold = 0;

	return(fold++);
#else
	ulint	ret;

	switch (lock_get_type(lock)) {
	case LOCK_REC:
		ut_a(heap_no != ULINT_UNDEFINED);

		ret = ut_fold_ulint_pair((ulint) lock_get_trx_id(lock),
					 lock_rec_get_space_id(lock));

		ret = ut_fold_ulint_pair(ret,
					 lock_rec_get_page_no(lock));

		ret = ut_fold_ulint_pair(ret, heap_no);

		break;
	case LOCK_TABLE:
		/* this check is actually not necessary for continuing
		correct operation, but something must have gone wrong if
		it fails. */
		ut_a(heap_no == ULINT_UNDEFINED);

		ret = (ulint) lock_get_table_id(lock);

		break;
	default:
		ut_error;
	}

	return(ret);
#endif
}

/***********************************************************************
Checks whether i_s_locks_row_t object represents a lock_t object. */
static
ibool
locks_row_eq_lock(
/*==============*/
					/* out: TRUE if they match */
	const i_s_locks_row_t*	row,	/* in: innodb_locks row */
	const lock_t*		lock,	/* in: lock object */
	ulint			heap_no)/* in: lock's record number
					or ULINT_UNDEFINED if the lock
					is a table lock */
{
#ifdef TEST_NO_LOCKS_ROW_IS_EVER_EQUAL_TO_LOCK_T
	return(0);
#else
	switch (lock_get_type(lock)) {
	case LOCK_REC:
		ut_a(heap_no != ULINT_UNDEFINED);

		return(row->lock_trx_id == lock_get_trx_id(lock)
		       && row->lock_space == lock_rec_get_space_id(lock)
		       && row->lock_page == lock_rec_get_page_no(lock)
		       && row->lock_rec == heap_no);

	case LOCK_TABLE:
		/* this check is actually not necessary for continuing
		correct operation, but something must have gone wrong if
		it fails. */
		ut_a(heap_no == ULINT_UNDEFINED);

		return(row->lock_trx_id == lock_get_trx_id(lock)
		       && row->lock_table_id == lock_get_table_id(lock));

	default:
		ut_error;
	}
#endif
}

/***********************************************************************
Searches for a row in the innodb_locks cache that has a specified id.
This happens in O(1) time since a hash table is used. Returns pointer to
the row or NULL if none is found. */
static
i_s_locks_row_t*
search_innodb_locks(
/*================*/
					/* out: row or NULL */
	trx_i_s_cache_t*	cache,	/* in: cache */
	const lock_t*		lock,	/* in: lock to search for */
	ulint			heap_no)/* in: lock's record number
					or ULINT_UNDEFINED if the lock
					is a table lock */
{
	i_s_hash_chain_t*	hash_chain;

	HASH_SEARCH(
		/* hash_chain->"next" */
		next,
		/* the hash table */
		cache->locks_hash,
		/* fold */
		fold_lock(lock, heap_no),
		/* the type of the next variable */
		i_s_hash_chain_t*,
		/* auxiliary variable */
		hash_chain,
		/* this determines if we have found the lock */
		locks_row_eq_lock(hash_chain->value, lock, heap_no));

	if (hash_chain == NULL) {

		return(NULL);
	}
	/* else */

	return(hash_chain->value);
}

/***********************************************************************
Adds new element to the locks cache, enlarging it if necessary.
Returns a pointer to the added row. If the row is already present then
no row is added and a pointer to the existing row is returned. */
static
i_s_locks_row_t*
add_lock_to_cache(
/*==============*/
					/* out: row */
	trx_i_s_cache_t*	cache,	/* in/out: cache */
	const lock_t*		lock,	/* in: the element to add */
	ulint			heap_no)/* in: lock's record number
					or ULINT_UNDEFINED if the lock
					is a table lock */
{
	i_s_locks_row_t*	dst_row;

#ifdef TEST_ADD_EACH_LOCKS_ROW_MANY_TIMES
	ulint	i;
	for (i = 0; i < 10000; i++) {
#endif
#ifndef TEST_DO_NOT_CHECK_FOR_DUPLICATE_ROWS
	/* quit if this lock is already present */
	dst_row = search_innodb_locks(cache, lock, heap_no);
	if (dst_row != NULL) {

		return(dst_row);
	}
#endif

	dst_row = (i_s_locks_row_t*)
		table_cache_create_empty_row(&cache->innodb_locks);

	fill_locks_row(dst_row, lock, heap_no);

#ifndef TEST_DO_NOT_INSERT_INTO_THE_HASH_TABLE
	HASH_INSERT(
		/* the type used in the hash chain */
		i_s_hash_chain_t,
		/* hash_chain->"next" */
		next,
		/* the hash table */
		cache->locks_hash,
		/* fold */
		fold_lock(lock, heap_no),
		/* add this data to the hash */
		&dst_row->hash_chain);
#endif
#ifdef TEST_ADD_EACH_LOCKS_ROW_MANY_TIMES
	} /* for()-loop */
#endif

	return(dst_row);
}

/***********************************************************************
Adds new pair of locks to the lock waits cache. */
static
void
add_lock_wait_to_cache(
/*===================*/
	trx_i_s_cache_t*	cache,		/* in/out: cache */
	const i_s_locks_row_t*	wait_lock_row,	/* in: pointer to the
						relevant wait-lock
						row in innodb_locks */
	const i_s_locks_row_t*	waited_lock_row)/* in: pointer to the
						relevant waited-lock
						row in innodb_locks */
{
	i_s_lock_waits_row_t*	dst_row;

	dst_row = (i_s_lock_waits_row_t*)
		table_cache_create_empty_row(&cache->innodb_lock_waits);

	fill_lock_waits_row(dst_row, wait_lock_row, waited_lock_row);
}

/***********************************************************************
Adds transaction's relevant (important) locks to cache.
If the transaction is waiting, then the wait lock is added to
innodb_locks and a pointer to the added row is returned in
wait_lock_row, otherwise wait_lock_row is set to NULL. */
static
void
add_trx_relevant_locks_to_cache(
/*============================*/
	trx_i_s_cache_t*	cache,	/* in/out: cache */
	const trx_t*		trx,	/* in: transaction */
	i_s_locks_row_t**	wait_lock_row)/* out: pointer to the
					wait lock row, or NULL */
{
	/* If transaction is waiting we add the wait lock and all locks
	from another transactions that are blocking the wait lock. */
	if (trx->que_state == TRX_QUE_LOCK_WAIT) {

		const lock_t*		curr_lock;
		ulint			wait_lock_heap_no;
		i_s_locks_row_t*	waited_lock_row;
		lock_queue_iterator_t	iter;

		ut_a(trx->wait_lock != NULL);

		wait_lock_heap_no
			= wait_lock_get_heap_no(trx->wait_lock);

		/* add the wait lock */
		*wait_lock_row
			= add_lock_to_cache(cache, trx->wait_lock,
					    wait_lock_heap_no);

		/* then iterate over the locks before the wait lock and
		add the ones that are blocking it */

		lock_queue_iterator_reset(&iter, trx->wait_lock,
					  ULINT_UNDEFINED);

		curr_lock = lock_queue_iterator_get_prev(&iter);
		while (curr_lock != NULL) {

			if (lock_has_to_wait(trx->wait_lock,
					     curr_lock)) {

				/* add the lock that trx->wait_lock is
				waiting for */
				waited_lock_row
					= add_lock_to_cache(
						cache, curr_lock,
						/* heap_no is the same
						for the wait and waited
						locks */
						wait_lock_heap_no);

				/* add the relation between both locks
				to innodb_lock_waits */
				add_lock_wait_to_cache(
					cache, *wait_lock_row,
					waited_lock_row);
			}

			curr_lock = lock_queue_iterator_get_prev(&iter);
		}
	} else {

		*wait_lock_row = NULL;
	}
}

/***********************************************************************
Checks if the cache can safely be updated. */
static
ibool
can_cache_be_updated(
/*=================*/
	const trx_i_s_cache_t*	cache)	/* in: cache */
{

	ullint	now;

/* The minimum time that a cache must not be updated after it has been
read for the last time; measured in microseconds. We use this technique
to ensure that SELECTs which join several INFORMATION SCHEMA tables read
the same version of the cache. */
#define CACHE_MIN_IDLE_TIME_US	100000 /* 0.1 sec */

	/* Here we read cache->last_read without acquiring its mutex
	because last_read is only updated when a shared rw lock on the
	whole cache is being held (see trx_i_s_cache_end_read()) and
	we are currently holding an exclusive rw lock on the cache.
	So it is not possible for last_read to be updated while we are
	reading it. */

#ifdef UNIV_SYNC_DEBUG
	ut_a(rw_lock_own(&cache->rw_lock, RW_LOCK_EX));
#endif

	now = ut_time_us(NULL);
	if (now - cache->last_read > CACHE_MIN_IDLE_TIME_US) {

		return(TRUE);
	}

	return(FALSE);
}

/***********************************************************************
Fetches the data needed to fill the 3 INFORMATION SCHEMA tables into the
table cache buffer. Cache must be locked for write. */
static
void
fetch_data_into_cache(
/*==================*/
	trx_i_s_cache_t*	cache)	/* in/out: cache */
{
	trx_t*			trx;
	i_s_trx_row_t*		trx_row;
	i_s_locks_row_t*	wait_lock_row;

	cache->innodb_trx.rows_used = 0;
	cache->innodb_locks.rows_used = 0;
	cache->innodb_lock_waits.rows_used = 0;
	hash_table_clear(cache->locks_hash);

	/* We iterate over the list of all transactions and add each one
	to innodb_trx's cache. We also add all locks that are relevant
	to each transaction into innodb_locks' and innodb_lock_waits'
	caches. */

	for (trx = UT_LIST_GET_FIRST(trx_sys->trx_list);
	     trx != NULL;
	     trx = UT_LIST_GET_NEXT(trx_list, trx)) {

		add_trx_relevant_locks_to_cache(cache, trx,
						&wait_lock_row);

		trx_row = (i_s_trx_row_t*)
			table_cache_create_empty_row(&cache->innodb_trx);

		fill_trx_row(trx_row, trx, wait_lock_row);
	}
}

/***********************************************************************
Update the transactions cache if it has not been read for some time.
Called from handler/i_s.cc. */

int
trx_i_s_possibly_fetch_data_into_cache(
/*===================================*/
					/* out: 0 - fetched, 1 - not */
	trx_i_s_cache_t*	cache)	/* in/out: cache */
{
	if (!can_cache_be_updated(cache)) {

		return(1);
	}

	/* We need to read trx_sys and record/table lock queues */
	mutex_enter(&kernel_mutex);

	fetch_data_into_cache(cache);

	mutex_exit(&kernel_mutex);

	return(0);
}

/***********************************************************************
Initialize INFORMATION SCHEMA trx related cache. */

void
trx_i_s_cache_init(
/*===============*/
	trx_i_s_cache_t*	cache)	/* out: cache to init */
{
	cache->last_read = 0;

	table_cache_init(&cache->innodb_trx, sizeof(i_s_trx_row_t));
	table_cache_init(&cache->innodb_locks, sizeof(i_s_locks_row_t));
	table_cache_init(&cache->innodb_lock_waits,
			 sizeof(i_s_lock_waits_row_t));

	cache->locks_hash = hash_create(LOCKS_HASH_CELLS_NUM);

	rw_lock_create(&cache->rw_lock, SYNC_INFORMATION_SCHEMA);

	mutex_create(&cache->last_read_mutex, SYNC_INFORMATION_SCHEMA);
}

/***********************************************************************
Issue a shared/read lock on the tables cache. */

void
trx_i_s_cache_start_read(
/*=====================*/
	trx_i_s_cache_t*	cache)	/* in: cache */
{
	rw_lock_s_lock(&cache->rw_lock);
}

/***********************************************************************
Release a shared/read lock on the tables cache. */

void
trx_i_s_cache_end_read(
/*===================*/
	trx_i_s_cache_t*	cache)	/* in: cache */
{
	ullint	now;

#ifdef UNIV_SYNC_DEBUG
	ut_a(rw_lock_own(&cache->rw_lock, RW_LOCK_SHARED));
#endif

	/* update cache last read time */
	now = ut_time_us(NULL);
	mutex_enter(&cache->last_read_mutex);
	cache->last_read = now;
	mutex_exit(&cache->last_read_mutex);

	rw_lock_s_unlock(&cache->rw_lock);
}

/***********************************************************************
Issue an exclusive/write lock on the tables cache. */

void
trx_i_s_cache_start_write(
/*======================*/
	trx_i_s_cache_t*	cache)	/* in: cache */
{
	rw_lock_x_lock(&cache->rw_lock);
}

/***********************************************************************
Release an exclusive/write lock on the tables cache. */

void
trx_i_s_cache_end_write(
/*====================*/
	trx_i_s_cache_t*	cache)	/* in: cache */
{
#ifdef UNIV_SYNC_DEBUG
	ut_a(rw_lock_own(&cache->rw_lock, RW_LOCK_EX));
#endif

	rw_lock_x_unlock(&cache->rw_lock);
}

/***********************************************************************
Selects a INFORMATION SCHEMA table cache from the whole cache. */
static
i_s_table_cache_t*
cache_select_table(
/*===============*/
					/* out: table cache */
	trx_i_s_cache_t*	cache,	/* in: whole cache */
	enum i_s_table		table)	/* in: which table */
{
	i_s_table_cache_t*	table_cache;

#ifdef UNIV_SYNC_DEBUG
	ut_a(rw_lock_own(&cache->rw_lock, RW_LOCK_SHARED)
	     || rw_lock_own(&cache->rw_lock, RW_LOCK_EX));
#endif

	switch (table) {
	case I_S_INNODB_TRX:
		table_cache = &cache->innodb_trx;
		break;
	case I_S_INNODB_LOCKS:
		table_cache = &cache->innodb_locks;
		break;
	case I_S_INNODB_LOCK_WAITS:
		table_cache = &cache->innodb_lock_waits;
		break;
	default:
		ut_error;
	}

	return(table_cache);
}

/***********************************************************************
Retrieves the number of used rows in the cache for a given
INFORMATION SCHEMA table. */

ullint
trx_i_s_cache_get_rows_used(
/*========================*/
					/* out: number of rows */
	trx_i_s_cache_t*	cache,	/* in: cache */
	enum i_s_table		table)	/* in: which table */
{
	i_s_table_cache_t*	table_cache;

	table_cache = cache_select_table(cache, table);

	return(table_cache->rows_used);
}

/***********************************************************************
Retrieves the nth row (zero-based) in the cache for a given
INFORMATION SCHEMA table. */

void*
trx_i_s_cache_get_nth_row(
/*======================*/
					/* out: row */
	trx_i_s_cache_t*	cache,	/* in: cache */
	enum i_s_table		table,	/* in: which table */
	ulint			n)	/* in: row number */
{
	i_s_table_cache_t*	table_cache;
	ulint			i;
	void*			row;

	table_cache = cache_select_table(cache, table);

	ut_a(n < table_cache->rows_used);

	row = NULL;

	for (i = 0; i < MEM_CHUNKS_IN_TABLE_CACHE; i++) {

		if (table_cache->chunks[i].offset
		    + table_cache->chunks[i].rows_allocd > n) {

			row = (char*) table_cache->chunks[i].base
				+ (n - table_cache->chunks[i].offset)
				* table_cache->row_size;
			break;
		}
	}

	ut_a(row != NULL);

	return(row);
}

/***********************************************************************
Crafts a lock id string from a i_s_locks_row_t object. Returns its
second argument. This function aborts if there is not enough space in
lock_id. Be sure to provide at least TRX_I_S_LOCK_ID_MAX_LEN if you want
to be 100% sure that it will not abort. */

char*
trx_i_s_create_lock_id(
/*===================*/
					/* out: resulting lock id */
	const i_s_locks_row_t*	row,	/* in: innodb_locks row */
	char*			lock_id,/* out: resulting lock_id */
	ulint			lock_id_size)/* in: size of the lock id
					buffer */
{
	int	res_len;

	/* please adjust TRX_I_S_LOCK_ID_MAX_LEN if you change this */

	if (row->lock_space != ULINT_UNDEFINED) {
		/* record lock */
		res_len = ut_snprintf(lock_id, lock_id_size,
				      "%llu:%lu:%lu:%lu",
				      row->lock_trx_id, row->lock_space,
				      row->lock_page, row->lock_rec);
	} else {
		/* table lock */
		res_len = ut_snprintf(lock_id, lock_id_size,
				      "%llu:%llu",
				      row->lock_trx_id,
				      row->lock_table_id);
	}

	/* the typecast is safe because snprintf(3) never returns
	negative result */
	ut_a(res_len >= 0);
	ut_a((ulint) res_len < lock_id_size);

	return(lock_id);
}
