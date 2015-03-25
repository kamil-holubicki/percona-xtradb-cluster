/* Copyright (c) 2015, 2015, Oracle and/or its affiliates. All rights reserved.

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

/* See http://code.google.com/p/googletest/wiki/Primer */

// First include (the generated) my_config.h, to get correct platform defines.
#include "my_config.h"

#define __STDC_LIMIT_MACROS

#include <gtest/gtest.h>

#include "univ.i"

#include "sync0policy.h" /* needed by ib0mutex.h, which is not self contained */
#include "ib0mutex.h" /* SysMutex */
#include "os0thread.h" /* os_thread_*() */
#include "srv0conc.h" /* srv_max_n_threads */
#include "sync0debug.h" /* sync_check_init(), sync_check_close() */
#include "sync0mutex.h" /* mutex_enter() */
#include "ut0lock_free_hash.h"

extern SysMutex	thread_mutex;

namespace innodb_lock_free_hash_unittest {

/** Generate a key to use in the (key, value) tuples.
@param[in]	i		some sequential number
@param[in]	extra_bits	extra bits to OR into the result
@return a key, derived from 'i' and 'extra_bits' */
inline
uintptr_t
key_gen(
	uintptr_t	i,
	uintptr_t	extra_bits)
{
	return((i * 7 + 3) | extra_bits);
}

/** Generate a value to use in the (key, value) tuples.
@param[in]	i	some sequential number
@return a value derived from 'i' */
inline
uintptr_t
val_from_i(
	uintptr_t	i)
{
	/* Make sure that the returned value is big enough, so that a few
	decrements don't make it negative. */
	return(i * 13 + 10000);
}

/** Insert some tuples in the hash, generating their keys and values
@param[in,out]	hash		hash into which to insert
@param[in]	n_elements	number of elements to insert
@param[in]	key_extra_bits	extra bits to use for key generation */
void
hash_insert(
	ut_lock_free_hash_t*	hash,
	uintptr_t		n_elements,
	uintptr_t		key_extra_bits)
{
	for (uintptr_t i = 0; i < n_elements; i++) {
		hash->set(key_gen(i, key_extra_bits), val_from_i(i));
	}
}

/** Check that the tuples inserted by hash_insert() are present in the hash.
@param[in]	hash		hash to check
@param[in]	n_elements	number of elements inserted by hash_insert()
@param[in]	key_extra_bits	extra bits that were given to hash_insert() */
void
hash_check_inserted(
	const ut_lock_free_hash_t*	hash,
	uintptr_t			n_elements,
	uintptr_t			key_extra_bits)
{
	for (uintptr_t i = 0; i < n_elements; i++) {
		const uintptr_t	key = key_gen(i, key_extra_bits);

		ASSERT_EQ(val_from_i(i), hash->get(key));
	}
}

TEST(ut0lock_free_hash, single_threaded)
{
	ut_lock_free_hash_t*	hash = new ut_lock_free_hash_t();

	const uintptr_t	n_elements = 700;

	hash_insert(hash, n_elements, 0);

	hash_check_inserted(hash, n_elements, 0);

	/* Increment the values of some and decrement of others. */
	for (uintptr_t i = 0; i < n_elements; i++) {

		const bool	should_inc = i % 2 == 0;
		const uintptr_t	key = key_gen(i, 0);

		/* Inc/dec from 0 to 9 times, depending on 'i'. */
		for (uintptr_t j = 0; j < i % 10; j++) {
			if (should_inc) {
				hash->inc(key);
			} else {
				hash->dec(key);
			}
		}
	}

	/* Check that increment/decrement was done properly. */
	for (uintptr_t i = 0; i < n_elements; i++) {

		const bool	was_inc = i % 2 == 0;
		const uintptr_t	delta = i % 10;

		ASSERT_EQ(val_from_i(i) + (was_inc ? delta : -delta),
			  hash->get(key_gen(i, 0)));
	}

	delete hash;
}

/** Global hash, edited from many threads concurrently. */
ut_lock_free_hash_t*	global_hash;

/** Number of common tuples (edited by all threads) to insert into the hash. */
static const uintptr_t	N_COMMON = 512;

/** Number of private, per-thread tuples to insert by each thread. */
static const uintptr_t	N_PRIV_PER_THREAD = 8;

/** Number of threads to start. Overall the hash will be filled with
N_COMMON + N_THREADS * N_PRIV_PER_THREAD tuples. */
static const uintptr_t	N_THREADS = 32;

/** Hammer the global hash with inc(), dec() and set(). The inc()/dec()
performed on the common keys will net to 0 when this thread ends. It also
inserts some tuples with keys that are unique to this thread.
@param[in]	arg	thread id, used to generate thread-private keys */
extern "C"
os_thread_ret_t
DECLARE_THREAD(thread)(
	void*	arg)
{
	const uintptr_t	thread_id = reinterpret_cast<uintptr_t>(arg);
	const uintptr_t	key_extra_bits = thread_id << 32;

	hash_insert(global_hash, N_PRIV_PER_THREAD, key_extra_bits);

	const uintptr_t	n_iter = 4096;

	for (uintptr_t i = 0; i < n_iter; i++) {
		for (uintptr_t j = 0; j < N_COMMON; j++) {
			const uintptr_t	key = key_gen(j, 0);

			global_hash->inc(key);
			global_hash->inc(key);
			global_hash->inc(key);

			global_hash->dec(key);
			global_hash->inc(key);

			global_hash->dec(key);
			global_hash->dec(key);
			global_hash->dec(key);
		}

		for (uintptr_t j = 0; j < N_PRIV_PER_THREAD; j++) {
			const uintptr_t	key = key_gen(j, key_extra_bits);

			for (uintptr_t k = 0; k < 4; k++) {
				global_hash->inc(key);
				global_hash->dec(key);
			}
		}
	}

	hash_check_inserted(global_hash, N_PRIV_PER_THREAD, key_extra_bits);

	os_thread_exit(NULL);

	OS_THREAD_DUMMY_RETURN;
}

TEST(ut0lock_free_hash, multi_threaded)
{
	srv_max_n_threads = 1024;

	sync_check_init();
	os_thread_init();

	global_hash = new ut_lock_free_hash_t();

	hash_insert(global_hash, N_COMMON, 0);

	for (uintptr_t i = 0; i < N_THREADS; i++) {
		os_thread_create(thread, reinterpret_cast<void*>(i), NULL);
	}

	/* Wait for all threads to exit. */
	mutex_enter(&thread_mutex);
	while (os_thread_count > 0) {
		mutex_exit(&thread_mutex);
		os_thread_sleep(100000 /* 0.1 sec */);
		mutex_enter(&thread_mutex);
	}
	mutex_exit(&thread_mutex);

	hash_check_inserted(global_hash, N_COMMON, 0);

	delete global_hash;

	os_thread_free();
	sync_check_close();
}

}
