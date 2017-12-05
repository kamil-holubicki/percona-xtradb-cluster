/* Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

#include "my_config.h"

#include <sys/types.h>

#include "my_dbug.h"
#include "my_inttypes.h"
#include "plugin/x/src/xpl_performance_schema.h"
#include "mysql/service_plugin_registry.h"
#include "sql/replication.h"
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#if !defined(_WIN32)
#include <sys/utsname.h>
#endif

#include <atomic>


typedef struct st_vio Vio;

const char  *my_localhost;
std::atomic<int32> connection_events_loop_aborted_flag;

int ip_to_hostname(struct sockaddr_storage*,
                   const char*,
                   char**,
                   uint*)
{
  DBUG_ASSERT(0);
  return 1;
}

int register_server_state_observer(Server_state_observer*, void*)
{
  return 0;
}

int unregister_server_state_observer(Server_state_observer*, void*)
{
  return 0;
}

extern "C"
void ssl_wrapper_version(Vio*, char*, const size_t)
{
}

extern "C"
void ssl_wrapper_cipher(Vio*, char*, const size_t)
{
}

extern "C"
long ssl_wrapper_cipher_list(Vio*, const char**, const size_t)
{
  return 0;
}

extern "C"
long ssl_wrapper_verify_depth(Vio*)
{
  return 0;
}

extern "C"
long ssl_wrapper_verify_mode(Vio*)
{
  return 0;
}

extern "C"
void ssl_wrapper_get_peer_certificate_issuer(Vio*, char*, const size_t)
{
}

extern "C"
void ssl_wrapper_get_peer_certificate_subject(Vio*, char*, const size_t)
{
}

extern "C"
long ssl_wrapper_get_verify_result_and_cert(Vio*)
{
  return 0;
}

extern "C"
long ssl_wrapper_ctx_verify_depth(struct st_VioSSLFd*)
{
  return 0;
}

extern "C"
long ssl_wrapper_ctx_verify_mode(struct st_VioSSLFd*)
{
  return 0;
}

extern "C"
void  ssl_wrapper_ctx_server_not_after(struct st_VioSSLFd*, char*, const size_t)
{
}

extern "C"
void ssl_wrapper_ctx_server_not_before(struct st_VioSSLFd*, char*, const size_t)
{
}

extern "C"
void ssl_wrapper_thread_cleanup()
{
}

extern "C"
long ssl_wrapper_sess_accept(struct st_VioSSLFd*)
{
  return 0;
}

extern "C"
long ssl_wrapper_sess_accept_good(struct st_VioSSLFd*)
{
  return 0;
}

SERVICE_TYPE(registry) * mysql_plugin_registry_acquire() { return nullptr; }

int mysql_plugin_registry_release(SERVICE_TYPE(registry) *) { return 0; }
