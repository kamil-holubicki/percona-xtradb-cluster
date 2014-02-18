/*****************************************************************************

Copyright (c) 2013, 2014, Oracle and/or its affiliates. All Rights Reserved.

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
@file include/page0size.h
A class describing a page size.

Created Nov 14, 2013 Vasil Dimov
*******************************************************/

#ifndef page0size_t
#define page0size_t

#include "univ.i"
#include "fsp0types.h"

#define PAGE_SIZE_T_SIZE_BITS	15

/** Page size descriptor. Contains the physical and logical page size, as well
as whether the page is compressed or not. */
class page_size_t {
public:
	/** Constructor from (physical, logical, is_compressed).
	@param[in]	physical	physical (on-disk) page size
	@param[in]	logical		logical (in-memory) page size
	@param[in]	is_compressed	whether the page is compressed */
	page_size_t(ulint physical, ulint logical, bool is_compressed)
		:
		m_physical(static_cast<unsigned>(physical)),
		m_logical(static_cast<unsigned>(logical)),
		m_is_compressed(static_cast<unsigned>(is_compressed))
	{
		ut_ad(physical <= (1 << PAGE_SIZE_T_SIZE_BITS));
		ut_ad(logical <= (1 << PAGE_SIZE_T_SIZE_BITS));

		ut_ad(ut_is_2pow(physical));
		ut_ad(ut_is_2pow(logical));

		ut_ad(logical <= UNIV_PAGE_SIZE_MAX);
		ut_ad(!is_compressed || physical <= UNIV_ZIP_SIZE_MAX);
	}

	/** Constructor from (fsp_flags).
	@param[in]	fsp_flags	filespace flags */
	explicit page_size_t(ulint fsp_flags)
	{
		ulint	ssize = FSP_FLAGS_GET_PAGE_SSIZE(fsp_flags);

		if (ssize != 0) {
			/* Convert from a 'log2 minus 9' to a page size
			in bytes. */
			const ulint	size
				= ((UNIV_ZIP_SIZE_MIN >> 1) << ssize);

			ut_ad(size <= UNIV_PAGE_SIZE_MAX);
			ut_ad(size <= (1 << PAGE_SIZE_T_SIZE_BITS));

			m_logical = size;
		} else {
			/* If the page size was not stored, then it
			is the legacy 16k. */
			m_logical = UNIV_PAGE_SIZE_ORIG;
		}

		ssize = FSP_FLAGS_GET_ZIP_SSIZE(fsp_flags);

		if (ssize == 0) {
			m_is_compressed = false;
			m_physical = m_logical;
		} else {
			m_is_compressed = true;

			/* Convert from a 'log2 minus 9' to a page size
			in bytes. */
			const ulint	phy
				= ((UNIV_ZIP_SIZE_MIN >> 1) << ssize);

			ut_ad(phy <= UNIV_ZIP_SIZE_MAX);
			ut_ad(phy <= (1 << PAGE_SIZE_T_SIZE_BITS));

			m_physical = phy;
		}
	}

	/** Retrieve the physical page size (on-disk).
	@return physical page size in bytes */
	inline ulint physical() const
	{
		/* Remove this assert once we add support for different
		page size per tablespace. Currently all tablespaces must
		have a page size that is equal to --innodb-page-size= */
		ut_ad(m_logical == srv_page_size);

		ut_ad(m_physical > 0);
		return(m_physical);
	}

	/** Retrieve the logical page size (in-memory).
	@return logical page size in bytes */
	inline ulint logical() const
	{
		ut_ad(m_logical > 0);
		return(m_logical);
	}

	/** Check whether the page is compressed on disk.
	@return true if compressed */
	inline bool is_compressed() const
	{
		return(m_is_compressed);
	}

	/** Copy the values from a given page_size_t object.
	@param[in]	src	page size object whose values to fetch */
	inline void copy_from(const page_size_t& src)
	{
		m_physical = src.physical();
		m_logical = src.logical();
		m_is_compressed = src.is_compressed();
	}

	/** Check if a given page_size_t object is equal to the current one.
	@param[in]	a	page_size_t object to compare
	@return true if equal */
	inline bool equals_to(const page_size_t& a) const
	{
		return(a.physical() == m_physical
		       && a.logical() == m_logical
		       && a.is_compressed() == m_is_compressed);
	}

private:

	/* Disable implicit copying. */
	void operator=(const page_size_t&);

	/* For non compressed tablespaces, physical page size is equal to
	the logical page size and the data is stored in buf_page_t::frame
	(and is also always equal to univ_page_size (--innodb-page-size=)).

	For compressed tablespaces, physical page size is the compressed
	page size as stored on disk and in buf_page_t::zip::data. The logical
	page size is the uncompressed page size in memory - the size of
	buf_page_t::frame (currently also always equal to univ_page_size
	(--innodb-page-size=)). */

	/** Physical page size. */
	unsigned	m_physical:PAGE_SIZE_T_SIZE_BITS;

	/** Logical page size. */
	unsigned	m_logical:PAGE_SIZE_T_SIZE_BITS;

	/** Flag designating whether the physical page is compressed, which is
	true IFF the whole tablespace where the page belongs is compressed. */
	unsigned	m_is_compressed:1;
};

extern page_size_t	univ_page_size;

#endif /* page0size_t */
