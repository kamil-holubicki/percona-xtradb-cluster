/*****************************************************************************

Copyright (c) 2014, 2014, Oracle and/or its affiliates. All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA

*****************************************************************************/

/**************************************************//**
@file ut/ut0stage.h
Supplementary code to performance schema stage instrumentation.

Created Nov 12, 2014 Vasil Dimov
*******************************************************/

#ifndef ut0stage_h
#define ut0stage_h

#include <algorithm>

#include "my_global.h" /* needed for headers from mysql/psi/ */

#include "mysql/psi/mysql_stage.h" /* mysql_stage_inc_work_completed */
#include "mysql/psi/psi.h" /* HAVE_PSI_STAGE_INTERFACE, PSI_stage_progress */

#include "univ.i"

#include "dict0mem.h" /* dict_index_t */
#include "row0log.h" /* row_log_estimate_work() */
#include "srv0srv.h" /* ut_stage_alter_t */

#ifdef HAVE_PSI_STAGE_INTERFACE

/** Class used to report ALTER TABLE progress via performance_schema.
The only user of this class is the ALTER TABLE code and it calls the methods
in the following order
constructor
begin_phase_read_pk()
  multiple times:
    n_pk_recs_inc() // once per record read
    inc() // once per page read
end_phase_read_pk()
if any new indexes are being added, for each one:
  begin_phase_sort()
    multiple times:
      inc() // once per record sorted
  begin_phase_insert()
    multiple times:
      inc() // once per record inserted
begin_phase_flush()
    multiple times:
      inc() // once per page flushed
begin_phase_log()
    multiple times:
      inc() // once per log-block applied
begin_phase_end()
destructor

This class knows the specifics of each phase and tries to increment the
progress in an even manner across the entire ALTER TABLE lifetime. */
class ut_stage_alter_t {
public:
	/** Constructor.
	@param[in]	pk	primary key of the old table */
	explicit
	ut_stage_alter_t(
		const dict_index_t*	pk)
		:
		m_pk(pk),
		m_n_pk_recs(0),
		m_n_pk_pages(0),
		m_n_recs_processed(0),
		m_n_flush_pages(0),
		m_cur_phase(NOT_STARTED)
	{
	}

	/** Destructor. */
	~ut_stage_alter_t();

	/** Flag an ALTER TABLE start (read primary key phase).
	@param[in]	n_sort_indexes	number of indexes that will be sorted
	during ALTER TABLE, used for estimating the total work to be done */
	void
	begin_phase_read_pk(
		ulint	n_sort_indexes);

	/** Increment the number of records in PK (table) with 1.
	This is used to get more accurate estimate about the number of
	records per page which is needed because some phases work on
	per-page basis while some work on per-record basis and we want
	to get the progress as even as possible. */
	void
	n_pk_recs_inc();

	/** Flag either one record or one page processed, depending on the
	current phase.
	@param[in]	inc_val	flag this many units processed at once */
	void
	inc(
		ulint	inc_val = 1);

	/** Flag the end of reading of the primary key.
	Here we know the exact number of pages and records and calculate
	the number of records per page and refresh the estimate. */
	void
	end_phase_read_pk();

	/** Flag the beginning of the sort phase.
	@param[in]	sort_multi_factor	since merge sort processes
	one page more than once we only update the estimate once per this
	many pages processed. */
	void
	begin_phase_sort(
		double	sort_multi_factor);

	/** Flag the beginning of the insert phase. */
	void
	begin_phase_insert();

	/** Flag the beginning of the flush phase.
	@param[in]	n_flush_pages	this many pages are going to be
	flushed */
	void
	begin_phase_flush(
		ulint	n_flush_pages);

	/** Flag the beginning of the log phase. */
	void
	begin_phase_log();

	/** Flag the beginning of the end phase. */
	void
	begin_phase_end();

private:

	/** Update the estimate of total work to be done. */
	void
	reestimate();

	/** Change the current phase.
	@param[in]	new_stage	pointer to the new stage to change to */
	void
	change_phase(
		const PSI_stage_info*	new_stage);

	/** Performance schema accounting object. */
	PSI_stage_progress*	m_progress;

	/** Old table PK. Used for calculating the estimate. */
	const dict_index_t*	m_pk;

	/** Number of records in the primary key (table). */
	ulint			m_n_pk_recs;

	/** Number of leaf pages in the primary key. */
	ulint			m_n_pk_pages;

	/** Estimated number of records per page. */
	ulint			m_n_recs_per_page;

	/** Number of indexes that are being added. */
	ulint			m_n_sort_indexes;

	/** During the sort phase, increment the counter once per this
	many pages processed. This is because sort processes one page more
	than once. */
	ulint			m_sort_multi_factor;

	/** Number of records processed during sort & insert phases. We
	need to increment the counter only once page, or once per
	recs-per-page records. */
	ulint			m_n_recs_processed;

	/** Number of pages to flush. */
	ulint			m_n_flush_pages;

	/** Current phase. */
	enum {
		NOT_STARTED = 0,
		READ_PK = 1,
		SORT = 2,
		INSERT = 3,
		FLUSH = 4,
		LOG = 5,
		END = 6,
	}			m_cur_phase;
};

/** Destructor. */
inline
ut_stage_alter_t::~ut_stage_alter_t()
{
	if (m_progress == NULL) {
		return;
	}

	/* Set completed = estimated before we quit. */
	mysql_stage_set_work_completed(
		m_progress,
		mysql_stage_get_work_estimated(m_progress));

	mysql_end_stage();
}

/** Flag an ALTER TABLE start (read primary key phase).
@param[in]	n_sort_indexes	number of indexes that will be sorted
during ALTER TABLE, used for estimating the total work to be done */
inline
void
ut_stage_alter_t::begin_phase_read_pk(
	ulint	n_sort_indexes)
{
	m_n_sort_indexes = n_sort_indexes;

	m_cur_phase = READ_PK;

	m_progress = mysql_set_stage(
		srv_stage_alter_table_read_pk.m_key);

	mysql_stage_set_work_completed(m_progress, 0);

	reestimate();
}

/** Increment the number of records in PK (table) with 1.
This is used to get more accurate estimate about the number of
records per page which is needed because some phases work on
per-page basis while some work on per-record basis and we want
to get the progress as even as possible. */
inline
void
ut_stage_alter_t::n_pk_recs_inc()
{
	m_n_pk_recs++;
}

/** Flag either one record or one page processed, depending on the
current phase.
@param[in]	inc_val	flag this many units processed at once */
inline
void
ut_stage_alter_t::inc(
	ulint	inc_val /* = 1 */)
{
	if (m_progress == NULL) {
		return;
	}

	ulint	multi_factor = 1;
	bool	should_proceed = true;

	switch (m_cur_phase) {
	case NOT_STARTED:
		ut_error;
	case READ_PK:
		m_n_pk_pages++;
		break;
	case SORT:
		multi_factor = m_sort_multi_factor;
		/* fall through */
	case INSERT: {
		/* Increment the progress every nth record. During
		sort and insert phases, this method is called once per
		record processed. */
		ulint	every_nth = m_n_recs_per_page * multi_factor;

		should_proceed = m_n_recs_processed++ % every_nth == 0;

		break;
	}
	case FLUSH:
		break;
	case LOG:
		break;
	case END:
		break;
	}

	if (should_proceed) {
		mysql_stage_inc_work_completed(m_progress, inc_val);
		reestimate();
	}
}

/** Flag the end of reading of the primary key.
Here we know the exact number of pages and records and calculate
the number of records per page and refresh the estimate. */
inline
void
ut_stage_alter_t::end_phase_read_pk()
{
	reestimate();

	if (m_n_pk_pages == 0) {
		m_n_recs_per_page = 1;
	} else {
		m_n_recs_per_page = std::max(
			1UL, m_n_pk_recs / m_n_pk_pages);
	}
}

/** Flag the beginning of the sort phase.
@param[in]	sort_multi_factor	since merge sort processes
one page more than once we only update the estimate once per this
many pages processed. */
inline
void
ut_stage_alter_t::begin_phase_sort(
	double	sort_multi_factor)
{
	if (sort_multi_factor <= 1) {
		m_sort_multi_factor = 1;
	} else {
		m_sort_multi_factor = static_cast<ulint>(
			round(m_sort_multi_factor));
	}

	change_phase(&srv_stage_alter_table_sort);
}

/** Flag the beginning of the insert phase. */
inline
void
ut_stage_alter_t::begin_phase_insert()
{
	change_phase(&srv_stage_alter_table_insert);
}

/** Flag the beginning of the flush phase.
@param[in]	n_flush_pages	this many pages are going to be
flushed */
inline
void
ut_stage_alter_t::begin_phase_flush(
	ulint	n_flush_pages)
{
	m_n_flush_pages = n_flush_pages;

	reestimate();

	change_phase(&srv_stage_alter_table_flush);
}

/** Flag the beginning of the log phase. */
inline
void
ut_stage_alter_t::begin_phase_log()
{
	change_phase(&srv_stage_alter_table_log);
}

/** Flag the beginning of the end phase. */
inline
void
ut_stage_alter_t::begin_phase_end()
{
	change_phase(&srv_stage_alter_table_end);
}

/** Update the estimate of total work to be done. */
inline
void
ut_stage_alter_t::reestimate()
{
	if (m_progress == NULL) {
		return;
	}

	/* During the log phase we calculate the estimate as
	work done so far + log size remaining. */
	if (m_cur_phase == LOG) {
		mysql_stage_set_work_estimated(
			m_progress,
			mysql_stage_get_work_completed(m_progress)
			+ row_log_estimate_work(m_pk));
		return;
	}

	/* During the other phases we use a formula, regardless of
	how much work has been done so far. */

	/* For number of pages in the PK - if the PK has not been
	read yet, use stat_n_leaf_pages (approximate), otherwise
	use the exact number we gathered. */
	const ulint	n_pk_pages
		= m_cur_phase != READ_PK
		? m_n_pk_pages
		: m_pk->stat_n_leaf_pages;

	/* If flush phase has not started yet and we do not know how
	many pages are to be flushed, then use a wild guess - the
	number of pages in the PK / 2. */
	if (m_n_flush_pages == 0) {
		m_n_flush_pages = n_pk_pages / 2;
	}

	ulonglong	estimate
		= n_pk_pages
		* (1 /* read PK */
		   /* sort & insert per created index */
		   + m_n_sort_indexes * 2)
		+ m_n_flush_pages
		+ row_log_estimate_work(m_pk);

	/* Prevent estimate < completed */
	estimate = std::max(estimate,
			    mysql_stage_get_work_completed(m_progress));

	mysql_stage_set_work_estimated(m_progress, estimate);
}

/** Change the current phase.
@param[in]	new_stage	pointer to the new stage to change to */
inline
void
ut_stage_alter_t::change_phase(
	const PSI_stage_info*	new_stage)
{
	if (m_progress == NULL) {
		return;
	}

	if (new_stage == &srv_stage_alter_table_read_pk) {
		m_cur_phase = READ_PK;
	} else if (new_stage == &srv_stage_alter_table_sort) {
		m_cur_phase = SORT;
	} else if (new_stage == &srv_stage_alter_table_insert) {
		m_cur_phase = INSERT;
	} else if (new_stage == &srv_stage_alter_table_flush) {
		m_cur_phase = FLUSH;
	} else if (new_stage == &srv_stage_alter_table_log) {
		m_cur_phase = LOG;
	} else if (new_stage == &srv_stage_alter_table_end) {
		m_cur_phase = END;
	} else {
		ut_error;
	}

	const ulonglong	c = mysql_stage_get_work_completed(m_progress);
	const ulonglong	e = mysql_stage_get_work_estimated(m_progress);

	m_progress = mysql_set_stage(new_stage->m_key);

	mysql_stage_set_work_completed(m_progress, c);
	mysql_stage_set_work_estimated(m_progress, e);
}
#else /* HAVE_PSI_STAGE_INTERFACE */

class ut_stage_alter_t {
public:
	explicit
	ut_stage_alter_t(
		const dict_index_t*	pk)
	{
	}

	void
	begin_phase_read_pk(
		ulint	n_sort_indexes)
	{
	}

	void
	n_pk_recs_inc()
	{
	}

	void
	inc(
		ulint	inc_val = 1)
	{
	}

	void
	end_phase_read_pk()
	{
	}

	void
	begin_phase_sort(
		double	sort_multi_factor)
	{
	}

	void
	begin_phase_insert()
	{
	}

	void
	begin_phase_flush(
		ulint	n_flush_pages)
	{
	}

	void
	begin_phase_log()
	{
	}

	void
	begin_phase_end()
	{
	}
};

#endif /* HAVE_PSI_STAGE_INTERFACE */

#endif /* ut0stage_h */
