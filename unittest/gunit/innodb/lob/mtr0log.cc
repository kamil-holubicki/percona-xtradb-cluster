/*****************************************************************************

Copyright (c) 2016, 2017 Oracle and/or its affiliates. All Rights Reserved.

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
#include "mach0data.h"
#include "lot0types.h"
#include "mtr0types.h"

void
mlog_write_ulint(
        byte*           ptr,    /*!< in: pointer where to write */
        ulint           val,    /*!< in: value to write */
        mlog_id_t       type)   /*!< in: MLOG_1BYTE, MLOG_2BYTES, MLOG_4BYTES */
{
        switch (type) {
        case MLOG_1BYTE:
                mach_write_to_1(ptr, val);
                break;
        case MLOG_2BYTES:
                mach_write_to_2(ptr, val);
                break;
        case MLOG_4BYTES:
                mach_write_to_4(ptr, val);
                break;
        default:
                ut_error;
        }
}
