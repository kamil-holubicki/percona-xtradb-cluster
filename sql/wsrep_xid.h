/* Copyright (C) 2015 Codership Oy <info@codership.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef WSREP_XID_H
#define WSREP_XID_H

#include "handler.h"  // XID typedef
#include "wsrep-lib/include/wsrep/gtid.hpp"

#define WSREP_XID_PREFIX "WSREPXid"
#define WSREP_XID_PREFIX_LEN 8
#define WSREP_XID_VERSIONED_PREFIX "WSREPXi"
#define WSREP_XID_VERSIONED_PREFIX_LEN 7
#define WSREP_XID_VERSION_OFFSET WSREP_XID_VERSIONED_PREFIX_LEN
#define WSREP_XID_VERSION_1 'd'
#define WSREP_XID_VERSION_2 'e'
#define WSREP_XID_VERSION_3 'f'
#define WSREP_XID_VERSION_4 'g'
#define WSREP_XID_VERSION_5 'h'
#define WSREP_XID_UUID_OFFSET 8
#define WSREP_XID_SEQNO_OFFSET (WSREP_XID_UUID_OFFSET + sizeof(wsrep_uuid_t))
#define WSREP_XID_GTRID_LEN (WSREP_XID_SEQNO_OFFSET + sizeof(wsrep_seqno_t))
#define WSREP_XID_GTRID_LEN_V_1_2 WSREP_XID_GTRID_LEN
#define WSREP_XID_GTRID_LEN_V_3_4_5 64
#define WSREP_XID_BQUAL_LEN_V_5 4

void wsrep_xid_init(xid_t *, const wsrep::gtid &);
int wsrep_is_wsrep_xid(const void *xid);
const wsrep::id &wsrep_xid_uuid(const XID &);
wsrep::seqno wsrep_xid_seqno(const XID &);

wsrep::gtid wsrep_get_SE_checkpoint();
bool wsrep_set_SE_checkpoint(const wsrep::gtid &gtid);

#endif /* WSREP_UTILS_H */
