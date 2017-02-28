/*****************************************************************************

Copyright (c) 1994, 2017, Oracle and/or its affiliates. All Rights Reserved.

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

#include "my_compiler.h"
#include "my_inttypes.h"

/********************************************************************//**
@file rem/rec.h
Record manager

Created 5/30/1994 Heikki Tuuri
*************************************************************************/

/** NOTE: The functions in this file should only use functions from
other files in library. The code in this file is used to make a library for
external tools. */

#ifndef rem_rec_h
#define rem_rec_h

#include "dict0boot.h"
#include "dict0dict.h"

/* Compact flag ORed to the extra size returned by rec_get_offsets() */
#define REC_OFFS_COMPACT	((ulint) 1 << 31)
/* SQL NULL flag in offsets returned by rec_get_offsets() */
#define REC_OFFS_SQL_NULL	((ulint) 1 << 31)
/* External flag in offsets returned by rec_get_offsets() */
#define REC_OFFS_EXTERNAL	((ulint) 1 << 30)
/* Mask for offsets returned by rec_get_offsets() */
#define REC_OFFS_MASK		(REC_OFFS_EXTERNAL - 1)

/* The offset of heap_no in a compact record */
#define REC_NEW_HEAP_NO		4
/* The shift of heap_no in a compact record.
The status is stored in the low-order bits. */
#define	REC_HEAP_NO_SHIFT	3

/* We list the byte offsets from the origin of the record, the mask,
and the shift needed to obtain each bit-field of the record. */

#define REC_NEXT		2
#define REC_NEXT_MASK		0xFFFFUL
#define REC_NEXT_SHIFT		0

#define REC_OLD_SHORT		3	/* This is single byte bit-field */
#define REC_OLD_SHORT_MASK	0x1UL
#define REC_OLD_SHORT_SHIFT	0

#define REC_OLD_N_FIELDS	4
#define REC_OLD_N_FIELDS_MASK	0x7FEUL
#define REC_OLD_N_FIELDS_SHIFT	1

#define REC_NEW_STATUS		3	/* This is single byte bit-field */
#define REC_NEW_STATUS_MASK	0x7UL
#define REC_NEW_STATUS_SHIFT	0

#define REC_OLD_HEAP_NO		5
#define REC_HEAP_NO_MASK	0xFFF8UL
#if 0 /* defined in rem0rec.h for use of page0zip.cc */
#define REC_NEW_HEAP_NO		4
#define	REC_HEAP_NO_SHIFT	3
#endif

#define REC_OLD_N_OWNED		6	/* This is single byte bit-field */
#define REC_NEW_N_OWNED		5	/* This is single byte bit-field */
#define	REC_N_OWNED_MASK	0xFUL
#define REC_N_OWNED_SHIFT	0

#define REC_OLD_INFO_BITS	6	/* This is single byte bit-field */
#define REC_NEW_INFO_BITS	5	/* This is single byte bit-field */
#define	REC_INFO_BITS_MASK	0xF0UL
#define REC_INFO_BITS_SHIFT	0

#if REC_OLD_SHORT_MASK << (8 * (REC_OLD_SHORT - 3)) \
		^ REC_OLD_N_FIELDS_MASK << (8 * (REC_OLD_N_FIELDS - 4)) \
		^ REC_HEAP_NO_MASK << (8 * (REC_OLD_HEAP_NO - 4)) \
		^ REC_N_OWNED_MASK << (8 * (REC_OLD_N_OWNED - 3)) \
		^ REC_INFO_BITS_MASK << (8 * (REC_OLD_INFO_BITS - 3)) \
		^ 0xFFFFFFFFUL
# error "sum of old-style masks != 0xFFFFFFFFUL"
#endif
#if REC_NEW_STATUS_MASK << (8 * (REC_NEW_STATUS - 3)) \
		^ REC_HEAP_NO_MASK << (8 * (REC_NEW_HEAP_NO - 4)) \
		^ REC_N_OWNED_MASK << (8 * (REC_NEW_N_OWNED - 3)) \
		^ REC_INFO_BITS_MASK << (8 * (REC_NEW_INFO_BITS - 3)) \
		^ 0xFFFFFFUL
# error "sum of new-style masks != 0xFFFFFFUL"
#endif

/* Info bit denoting the predefined minimum record: this bit is set
if and only if the record is the first user record on a non-leaf
B-tree page that is the leftmost page on its level
(PAGE_LEVEL is nonzero and FIL_PAGE_PREV is FIL_NULL). */
#define REC_INFO_MIN_REC_FLAG	0x10UL
/* The deleted flag in info bits */
#define REC_INFO_DELETED_FLAG	0x20UL	/* when bit is set to 1, it means the
					record has been delete marked */

/* Number of extra bytes in an old-style record,
in addition to the data and the offsets */
#define REC_N_OLD_EXTRA_BYTES	6
/* Number of extra bytes in a new-style record,
in addition to the data and the offsets */
#define REC_N_NEW_EXTRA_BYTES	5

/* Record status values */
#define REC_STATUS_ORDINARY	0
#define REC_STATUS_NODE_PTR	1
#define REC_STATUS_INFIMUM	2
#define REC_STATUS_SUPREMUM	3

/* The following four constants are needed in page0zip.cc in order to
efficiently compress and decompress pages. */

/* Length of a B-tree node pointer, in bytes */
#define REC_NODE_PTR_SIZE	4

/** SQL null flag in a 1-byte offset of ROW_FORMAT=REDUNDANT records */
#define REC_1BYTE_SQL_NULL_MASK	0x80UL
/** SQL null flag in a 2-byte offset of ROW_FORMAT=REDUNDANT records */
#define REC_2BYTE_SQL_NULL_MASK	0x8000UL

/** In a 2-byte offset of ROW_FORMAT=REDUNDANT records, the second most
significant bit denotes that the tail of a field is stored off-page. */
#define REC_2BYTE_EXTERN_MASK	0x4000UL

#ifdef UNIV_DEBUG
/* Length of the rec_get_offsets() header */
# define REC_OFFS_HEADER_SIZE	4
#else /* UNIV_DEBUG */
/* Length of the rec_get_offsets() header */
# define REC_OFFS_HEADER_SIZE	2
#endif /* UNIV_DEBUG */

/* Number of elements that should be initially allocated for the
offsets[] array, first passed to rec_get_offsets() */
#define REC_OFFS_NORMAL_SIZE	100
#define REC_OFFS_SMALL_SIZE	10

/* Get the base address of offsets.  The extra_size is stored at
this position, and following positions hold the end offsets of
the fields. */
#define rec_offs_base(offsets) (offsets + REC_OFFS_HEADER_SIZE)

/******************************************************//**
The following function determines the offsets to each field
in the record.	It can reuse a previously allocated array.
@return the new offsets */
ulint*
rec_get_offsets_func(
/*=================*/
	const rec_t*		rec,	/*!< in: physical record */
	const dict_index_t*	index,	/*!< in: record descriptor */
	ulint*			offsets,/*!< in/out: array consisting of
					offsets[0] allocated elements,
					or an array from rec_get_offsets(),
					or NULL */
	ulint			n_fields,/*!< in: maximum number of
					initialized fields
					 (ULINT_UNDEFINED if all fields) */
#ifdef UNIV_DEBUG
	const char*		file,	/*!< in: file name where called */
	ulint			line,	/*!< in: line number where called */
#endif /* UNIV_DEBUG */
	mem_heap_t**		heap)	/*!< in/out: memory heap */
	MY_ATTRIBUTE((warn_unused_result));

/******************************************************//**
The following function determines the offsets to each field
in the record.  It can reuse a previously allocated array. */
void
rec_get_offsets_reverse(
/*====================*/
	const byte*		extra,	/*!< in: the extra bytes of a
					compact record in reverse order,
					excluding the fixed-size
					REC_N_NEW_EXTRA_BYTES */
	const dict_index_t*	index,	/*!< in: record descriptor */
	ulint			node_ptr,/*!< in: nonzero=node pointer,
					0=leaf node */
	ulint*			offsets);/*!< in/out: array consisting of
					offsets[0] allocated elements */

/******************************************************//**
Gets a bit field from within 1 byte. */
UNIV_INLINE
ulint
rec_get_bit_field_1(
/*================*/
	const rec_t*	rec,	/*!< in: pointer to record origin */
	ulint		offs,	/*!< in: offset from the origin down */
	ulint		mask,	/*!< in: mask used to filter bits */
	ulint		shift)	/*!< in: shift right applied after masking */
{
	ut_ad(rec);

	return((mach_read_from_1(rec - offs) & mask) >> shift);
}

/******************************************************//**
Gets a bit field from within 2 bytes. */
UNIV_INLINE
ulint
rec_get_bit_field_2(
/*================*/
	const rec_t*	rec,	/*!< in: pointer to record origin */
	ulint		offs,	/*!< in: offset from the origin down */
	ulint		mask,	/*!< in: mask used to filter bits */
	ulint		shift)	/*!< in: shift right applied after masking */
{
	ut_ad(rec);

	return((mach_read_from_2(rec - offs) & mask) >> shift);
}

/******************************************************//**
The following function retrieves the status bits of a new-style record.
@return status bits */
UNIV_INLINE MY_ATTRIBUTE((warn_unused_result))
ulint
rec_get_status(
/*===========*/
	const rec_t*	rec)	/*!< in: physical record */
{
	ulint	ret;

	ut_ad(rec);

	ret = rec_get_bit_field_1(rec, REC_NEW_STATUS,
				  REC_NEW_STATUS_MASK, REC_NEW_STATUS_SHIFT);
	ut_ad((ret & ~REC_NEW_STATUS_MASK) == 0);

	return(ret);
}

/******************************************************//**
The following function is used to get the number of fields
in an old-style record.
@return number of data fields */
UNIV_INLINE MY_ATTRIBUTE((warn_unused_result))
ulint
rec_get_n_fields_old(
/*=================*/
	const rec_t*	rec)	/*!< in: physical record */
{
	ulint	ret;

	ut_ad(rec);

	ret = rec_get_bit_field_2(rec, REC_OLD_N_FIELDS,
				  REC_OLD_N_FIELDS_MASK,
				  REC_OLD_N_FIELDS_SHIFT);
	ut_ad(ret <= REC_MAX_N_FIELDS);
	ut_ad(ret > 0);

	return(ret);
}

/******************************************************//**
The following function is used to get the number of fields
in a record.
@return number of data fields */
UNIV_INLINE
ulint
rec_get_n_fields(
/*=============*/
	const rec_t*		rec,	/*!< in: physical record */
	const dict_index_t*	index)	/*!< in: record descriptor */
{
	ut_ad(rec);
	ut_ad(index);

	if (!dict_table_is_comp(index->table)) {
		return(rec_get_n_fields_old(rec));
	}

	switch (rec_get_status(rec)) {
	case REC_STATUS_ORDINARY:
		return(dict_index_get_n_fields(index));
	case REC_STATUS_NODE_PTR:
		return(dict_index_get_n_unique_in_tree(index) + 1);
	case REC_STATUS_INFIMUM:
	case REC_STATUS_SUPREMUM:
		return(1);
	default:
		ut_error;
		return(ULINT_UNDEFINED);
	}
}

/** Confirms the n_fields of the entry is sane with comparing the other
record in the same page specified
@param[in]	index	index
@param[in]	rec	record of the same page
@param[in]	entry	index entry
@return	true if n_fields is sane */
UNIV_INLINE
bool
rec_n_fields_is_sane(
	dict_index_t*	index,
	const rec_t*	rec,
	const dtuple_t*	entry)
{
	return(rec_get_n_fields(rec, index)
	       == dtuple_get_n_fields(entry)
	       /* a record for older SYS_INDEXES table
	       (missing merge_threshold column) is acceptable. */
	       || (index->table->id == DICT_INDEXES_ID
		   && rec_get_n_fields(rec, index)
		      == dtuple_get_n_fields(entry) - 1));
}

/**********************************************************//**
The following function returns the number of allocated elements
for an array of offsets.
@return number of elements */
UNIV_INLINE MY_ATTRIBUTE((warn_unused_result))
ulint
rec_offs_get_n_alloc(
/*=================*/
	const ulint*	offsets)/*!< in: array for rec_get_offsets() */
{
	ulint	n_alloc;
	ut_ad(offsets);
	n_alloc = offsets[0];
	ut_ad(n_alloc > REC_OFFS_HEADER_SIZE);
	UNIV_MEM_ASSERT_W(offsets, n_alloc * sizeof *offsets);
	return(n_alloc);
}

/**********************************************************//**
The following function sets the number of allocated elements
for an array of offsets. */
UNIV_INLINE
void
rec_offs_set_n_alloc(
/*=================*/
	ulint*	offsets,	/*!< out: array for rec_get_offsets(),
				must be allocated */
	ulint	n_alloc)	/*!< in: number of elements */
{
	ut_ad(offsets);
	ut_ad(n_alloc > REC_OFFS_HEADER_SIZE);
	UNIV_MEM_ASSERT_AND_ALLOC(offsets, n_alloc * sizeof *offsets);
	offsets[0] = n_alloc;
}

/**********************************************************//**
The following function sets the number of fields in offsets. */
UNIV_INLINE
void
rec_offs_set_n_fields(
/*==================*/
	ulint*	offsets,	/*!< in/out: array returned by
				rec_get_offsets() */
	ulint	n_fields)	/*!< in: number of fields */
{
	ut_ad(offsets);
	ut_ad(n_fields > 0);
	ut_ad(n_fields <= REC_MAX_N_FIELDS);
	ut_ad(n_fields + REC_OFFS_HEADER_SIZE
	      <= rec_offs_get_n_alloc(offsets));
	offsets[1] = n_fields;
}

/**********************************************************//**
The following function returns the number of fields in a record.
@return number of fields */
UNIV_INLINE MY_ATTRIBUTE((warn_unused_result))
ulint
rec_offs_n_fields(
/*==============*/
	const ulint*	offsets)/*!< in: array returned by rec_get_offsets() */
{
	ulint	n_fields;
	ut_ad(offsets);
	n_fields = offsets[1];
	ut_ad(n_fields > 0);
	ut_ad(n_fields <= REC_MAX_N_FIELDS);
	ut_ad(n_fields + REC_OFFS_HEADER_SIZE
	      <= rec_offs_get_n_alloc(offsets));
	return(n_fields);
}

/******************************************************//**
The following function determines the offsets to each field in the
record.	 The offsets are written to a previously allocated array of
ulint, where rec_offs_n_fields(offsets) has been initialized to the
number of fields in the record.	 The rest of the array will be
initialized by this function.  rec_offs_base(offsets)[0] will be set
to the extra size (if REC_OFFS_COMPACT is set, the record is in the
new format; if REC_OFFS_EXTERNAL is set, the record contains externally
stored columns), and rec_offs_base(offsets)[1..n_fields] will be set to
offsets past the end of fields 0..n_fields, or to the beginning of
fields 1..n_fields+1.  When the high-order bit of the offset at [i+1]
is set (REC_OFFS_SQL_NULL), the field i is NULL.  When the second
high-order bit of the offset at [i+1] is set (REC_OFFS_EXTERNAL), the
field i is being stored externally. */
void
rec_init_offsets(
/*=============*/
	const rec_t*		rec,	/*!< in: physical record */
	const dict_index_t*	index,	/*!< in: record descriptor */
	ulint*			offsets);/*!< in/out: array of offsets;
					in: n=rec_offs_n_fields(offsets) */

#ifdef UNIV_DEBUG
/************************************************************//**
Validates offsets returned by rec_get_offsets().
@return TRUE if valid */
UNIV_INLINE MY_ATTRIBUTE((warn_unused_result))
ibool
rec_offs_validate(
/*==============*/
	const rec_t*		rec,	/*!< in: record or NULL */
	const dict_index_t*	index,	/*!< in: record descriptor or NULL */
	const ulint*		offsets)/*!< in: array returned by
					rec_get_offsets() */
{
	ulint	i	= rec_offs_n_fields(offsets);
	ulint	last	= ULINT_MAX;
	ulint	comp	= *rec_offs_base(offsets) & REC_OFFS_COMPACT;

	if (rec) {
		ut_ad((ulint) rec == offsets[2]);
		if (!comp) {
			ut_a(rec_get_n_fields_old(rec) >= i);
		}
	}
	if (index) {
		ulint max_n_fields;
		ut_ad((ulint) index == offsets[3]);
		max_n_fields = ut_max(
			dict_index_get_n_fields(index),
			dict_index_get_n_unique_in_tree(index) + 1);
		if (comp && rec) {
			switch (rec_get_status(rec)) {
			case REC_STATUS_ORDINARY:
				break;
			case REC_STATUS_NODE_PTR:
				max_n_fields = dict_index_get_n_unique_in_tree(
					index) + 1;
				break;
			case REC_STATUS_INFIMUM:
			case REC_STATUS_SUPREMUM:
				max_n_fields = 1;
				break;
			default:
				ut_error;
			}
		}
		/* index->n_def == 0 for dummy indexes if !comp */
		ut_a(!comp || index->n_def);
		ut_a(!index->n_def || i <= max_n_fields);
	}
	while (i--) {
		ulint	curr = rec_offs_base(offsets)[1 + i] & REC_OFFS_MASK;
		ut_a(curr <= last);
		last = curr;
	}
	return(TRUE);
}

/************************************************************//**
Updates debug data in offsets, in order to avoid bogus
rec_offs_validate() failures. */
UNIV_INLINE
void
rec_offs_make_valid(
/*================*/
	const rec_t*		rec,	/*!< in: record */
	const dict_index_t*	index,	/*!< in: record descriptor */
	ulint*			offsets)/*!< in: array returned by
					rec_get_offsets() */
{
	ut_ad(rec);
	ut_ad(index);
	ut_ad(offsets);
	ut_ad(rec_get_n_fields(rec, index) >= rec_offs_n_fields(offsets));
	offsets[2] = (ulint) rec;
	offsets[3] = (ulint) index;
}
#else
# define rec_offs_make_valid(rec, index, offsets) ((void) 0)
#endif /* UNIV_DEBUG */

/******************************************************//**
Determine the offset to each field in a leaf-page record
in ROW_FORMAT=COMPACT.  This is a special case of
rec_init_offsets() and rec_get_offsets_func(). */
UNIV_INLINE
void
rec_init_offsets_comp_ordinary(
/*===========================*/
	const rec_t*		rec,	/*!< in: physical record in
					ROW_FORMAT=COMPACT */
	bool			temp,	/*!< in: whether to use the
					format for temporary files in
					index creation */
	const dict_index_t*	index,	/*!< in: record descriptor */
	ulint*			offsets)/*!< in/out: array of offsets;
					in: n=rec_offs_n_fields(offsets) */
{
	ulint		i		= 0;
	ulint		offs		= 0;
	ulint		any_ext		= 0;
	ulint		n_null		= index->n_nullable;
	const byte*	nulls		= temp
		? rec - 1
		: rec - (1 + REC_N_NEW_EXTRA_BYTES);
	const byte*	lens		= nulls - UT_BITS_IN_BYTES(n_null);
	ulint		null_mask	= 1;

#ifdef UNIV_DEBUG
	/* We cannot invoke rec_offs_make_valid() here if temp=true.
	Similarly, rec_offs_validate() will fail in that case, because
	it invokes rec_get_status(). */
	offsets[2] = (ulint) rec;
	offsets[3] = (ulint) index;
#endif /* UNIV_DEBUG */

	ut_ad(temp || dict_table_is_comp(index->table));

	if (temp && dict_table_is_comp(index->table)) {
		/* No need to do adjust fixed_len=0. We only need to
		adjust it for ROW_FORMAT=REDUNDANT. */
		temp = false;
	}

	/* read the lengths of fields 0..n */
	do {
		const dict_field_t*	field = index->get_field(i);
		const dict_col_t*	col = field->col;
		ulint			len;

		if (!(col->prtype & DATA_NOT_NULL)) {
			/* nullable field => read the null flag */
			ut_ad(n_null--);

			if (UNIV_UNLIKELY(!(byte) null_mask)) {
				nulls--;
				null_mask = 1;
			}

			if (*nulls & null_mask) {
				null_mask <<= 1;
				/* No length is stored for NULL fields.
				We do not advance offs, and we set
				the length to zero and enable the
				SQL NULL flag in offsets[]. */
				len = offs | REC_OFFS_SQL_NULL;
				goto resolved;
			}
			null_mask <<= 1;
		}

		if (!field->fixed_len
		    || (temp && !col->get_fixed_size(temp))) {
			ut_ad(col->mtype != DATA_POINT);
			/* Variable-length field: read the length */
			len = *lens--;
			/* If the maximum length of the field is up
			to 255 bytes, the actual length is always
			stored in one byte. If the maximum length is
			more than 255 bytes, the actual length is
			stored in one byte for 0..127.  The length
			will be encoded in two bytes when it is 128 or
			more, or when the field is stored externally. */
			if (DATA_BIG_COL(col)) {
				if (len & 0x80) {
					/* 1exxxxxxx xxxxxxxx */
					len <<= 8;
					len |= *lens--;

					offs += len & 0x3fff;
					if (UNIV_UNLIKELY(len
							  & 0x4000)) {
						ut_ad(index->is_clustered());
						any_ext = REC_OFFS_EXTERNAL;
						len = offs
							| REC_OFFS_EXTERNAL;
					} else {
						len = offs;
					}

					goto resolved;
				}
			}

			len = offs += len;
		} else {
			len = offs += field->fixed_len;
		}
resolved:
		rec_offs_base(offsets)[i + 1] = len;
	} while (++i < rec_offs_n_fields(offsets));

	*rec_offs_base(offsets)
		= (rec - (lens + 1)) | REC_OFFS_COMPACT | any_ext;
}

/******************************************************//**
The following function is used to test whether the data offsets in the record
are stored in one-byte or two-byte format.
@return TRUE if 1-byte form */
UNIV_INLINE MY_ATTRIBUTE((warn_unused_result))
ibool
rec_get_1byte_offs_flag(
/*====================*/
	const rec_t*	rec)	/*!< in: physical record */
{
#if TRUE != 1
#error "TRUE != 1"
#endif

	return(rec_get_bit_field_1(rec, REC_OLD_SHORT, REC_OLD_SHORT_MASK,
				   REC_OLD_SHORT_SHIFT));
}


/******************************************************//**
Returns the offset of nth field end if the record is stored in the 1-byte
offsets form. If the field is SQL null, the flag is ORed in the returned
value.
@return offset of the start of the field, SQL null flag ORed */
UNIV_INLINE MY_ATTRIBUTE((warn_unused_result))
ulint
rec_1_get_field_end_info(
/*=====================*/
	const rec_t*	rec,	/*!< in: record */
	ulint		n)	/*!< in: field index */
{
	ut_ad(rec_get_1byte_offs_flag(rec));
	ut_ad(n < rec_get_n_fields_old(rec));

	return(mach_read_from_1(rec - (REC_N_OLD_EXTRA_BYTES + n + 1)));
}

/******************************************************//**
Returns the offset of nth field end if the record is stored in the 2-byte
offsets form. If the field is SQL null, the flag is ORed in the returned
value.
@return offset of the start of the field, SQL null flag and extern
storage flag ORed */
UNIV_INLINE MY_ATTRIBUTE((warn_unused_result))
ulint
rec_2_get_field_end_info(
/*=====================*/
	const rec_t*	rec,	/*!< in: record */
	ulint		n)	/*!< in: field index */
{
	ut_ad(!rec_get_1byte_offs_flag(rec));
	ut_ad(n < rec_get_n_fields_old(rec));

	return(mach_read_from_2(rec - (REC_N_OLD_EXTRA_BYTES + 2 * n + 2)));
}
#endif
