/* Copyright (c) 2000, 2010, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   Without limiting anything contained in the foregoing, this file,
   which is part of C Driver for MySQL (Connector/C), is also subject to the
   Universal FOSS Exception, version 1.0, a copy of which can be found at
   http://oss.oracle.com/licenses/universal-foss-exception.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/* Wait a given number of microseconds */

#include "mysys_priv.h"
#include <m_string.h>

void my_sleep(ulong m_seconds)
{
#if defined(__WIN__)
  Sleep(m_seconds/1000+1);      /* Sleep() has millisecond arg */
#elif defined(HAVE_SELECT)
  struct timeval t;
  t.tv_sec=  m_seconds / 1000000L;
  t.tv_usec= m_seconds % 1000000L;
  select(0,0,0,0,&t); /* sleep */
#else
  uint sec=    (uint) (m_seconds / 1000000L);
  ulong start= (ulong) time((time_t*) 0);
  while ((ulong) time((time_t*) 0) < start+sec);
#endif
}
