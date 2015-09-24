/* Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA */

#include "my_global.h"

extern "C" void set_timespec_nsec(struct timespec *abstime, ulonglong nsec)
{
#ifdef HAVE_STRUCT_TIMESPEC
  ulonglong now= my_getsystime() + (nsec / 100);
  ulonglong tv_sec= now / 10000000ULL;
#if SIZEOF_TIME_T < SIZEOF_LONG_LONG
  /* Ensure that the number of seconds don't overflow. */
  tv_sec= MY_MIN(tv_sec, ((ulonglong)INT_MAX32));
#endif
  abstime->tv_sec=  (time_t)tv_sec;
  abstime->tv_nsec= (now % 10000000ULL) * 100 + (nsec % 100);
#else /* !HAVE_STRUCT_TIMESPEC */
  ulonglong max_timeout_msec= (nsec / 1000000);
  union ft64 tv;
  GetSystemTimeAsFileTime(&tv.ft);
  abstime->tv.i64= tv.i64 + (__int64)(nsec / 100);
#if SIZEOF_LONG < SIZEOF_LONG_LONG
  /* Ensure that the msec value doesn't overflow. */
  max_timeout_msec= MY_MIN(max_timeout_msec, ((ulonglong)INT_MAX32));
#endif
  abstime->max_timeout_msec= (long)max_timeout_msec;
#endif /* !HAVE_STRUCT_TIMESPEC */
}


extern "C" void set_timespec(struct timespec *abstime, ulonglong sec)
{
  set_timespec_nsec(abstime, sec * 1000000000ULL);
}
