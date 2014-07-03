# Copyright (c) 2010, 2014, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

# Avoid system checks on Windows by pre-caching  results. Most of the system checks
# are not relevant for Windows anyway and it takes lot more time to run them,
# since CMake creates a Visual Studio project for each tiny test.

#
# From configure.cmake, in order of appearance 
#
SET(HAVE_LLVM_LIBCPP CACHE  INTERNAL "")
# Hardcoded in configure.cmake _LARGEFILE_SOURCE
SET(CMAKE_HAVE_PTHREAD_H CACHE  INTERNAL "") # Only needed by CMake

# Libraries
# Not checked for Windows HAVE_LIBM
# Not checked for Windows HAVE_LIBNSL
# Not checked for Windows HAVE_LIBCRYPT
# Not checked for Windows HAVE_LIBSOCKET
# Not checked for Windows HAVE_LIBDL
# Not checked for Windows HAVE_LIBRT
# Not checked for Windows HAVE_LIBWRAP

# Header files
SET(HAVE_ALLOCA_H CACHE  INTERNAL "")
SET(HAVE_ARPA_INET_H CACHE  INTERNAL "")
SET(HAVE_CRYPT_H CACHE  INTERNAL "")
SET(HAVE_CXXABI_H CACHE INTERNAL "")
SET(HAVE_DIRENT_H CACHE  INTERNAL "")
SET(HAVE_DLFCN_H CACHE  INTERNAL "")
SET(HAVE_EXECINFO_H CACHE  INTERNAL "")
SET(HAVE_FPU_CONTROL_H CACHE  INTERNAL "")
SET(HAVE_GRP_H CACHE  INTERNAL "")
SET(HAVE_IEEEFP_H CACHE  INTERNAL "")
SET(HAVE_LANGINFO_H CACHE  INTERNAL "")
SET(HAVE_MALLOC_H 1 CACHE  INTERNAL "")
SET(HAVE_NETINET_IN_H CACHE  INTERNAL "")
SET(HAVE_PATHS_H CACHE  INTERNAL "")
SET(HAVE_POLL_H CACHE  INTERNAL "")
SET(HAVE_PWD_H CACHE  INTERNAL "")
SET(HAVE_SCHED_H CACHE  INTERNAL "")
SET(HAVE_STRINGS_H CACHE  INTERNAL "")
SET(HAVE_SYNCH_H CACHE  INTERNAL "")
SET(HAVE_SYS_CDEFS_H CACHE  INTERNAL "")
SET(HAVE_SYS_IOCTL_H CACHE  INTERNAL "")
SET(HAVE_SYS_MALLOC_H CACHE  INTERNAL "")
SET(HAVE_SYS_MMAN_H CACHE  INTERNAL "")
SET(HAVE_SYS_PRCTL_H CACHE  INTERNAL "")
SET(HAVE_SYS_RESOURCE_H CACHE  INTERNAL "")
SET(HAVE_SYS_SELECT_H CACHE  INTERNAL "")
SET(HAVE_SYS_SOCKET_H CACHE  INTERNAL "")
SET(HAVE_TERM_H CACHE  INTERNAL "")
SET(HAVE_ASM_TERMBITS_H CACHE INTERNAL "")
SET(HAVE_TERMIOS_H CACHE  INTERNAL "")
SET(HAVE_TERMIO_H CACHE  INTERNAL "")
SET(HAVE_TERMCAP_H CACHE  INTERNAL "")
SET(HAVE_UNISTD_H CACHE  INTERNAL "")
SET(HAVE_UTIME_H CACHE  INTERNAL "")
SET(HAVE_SYS_UTIME_H 1 CACHE  INTERNAL "")
SET(HAVE_SYS_WAIT_H CACHE  INTERNAL "")
SET(HAVE_SYS_PARAM_H CACHE  INTERNAL "")
SET(HAVE_SYS_VADVISE_H CACHE  INTERNAL "")
SET(HAVE_FNMATCH_H CACHE  INTERNAL "")
SET(HAVE_SYS_UN_H CACHE  INTERNAL "")
SET(HAVE_VIS_H CACHE INTERNAL "")
SET(HAVE_SASL_SASL_H CACHE  INTERNAL "")

# Libevent
SET(HAVE_DEVPOLL CACHE  INTERNAL "")
SET(HAVE_SYS_DEVPOLL_H CACHE  INTERNAL "")
SET(HAVE_SYS_EPOLL_H CACHE  INTERNAL "")
SET(HAVE_TAILQFOREACH CACHE INTERNAL "")

# Functions
SET(HAVE_ALIGNED_MALLOC 1 CACHE  INTERNAL "")
SET(HAVE_BACKTRACE CACHE  INTERNAL "")
SET(HAVE_PRINTSTACK CACHE  INTERNAL "")
SET(HAVE_INDEX CACHE  INTERNAL "")
SET(HAVE_CLOCK_GETTIME CACHE  INTERNAL "")
SET(HAVE_CUSERID CACHE  INTERNAL "")
SET(HAVE_DIRECTIO CACHE  INTERNAL "")
SET(HAVE_FTRUNCATE CACHE  INTERNAL "")
SET(HAVE_COMPRESS CACHE  INTERNAL "")
SET(HAVE_CRYPT CACHE  INTERNAL "")
SET(HAVE_DLOPEN CACHE  INTERNAL "")
SET(HAVE_FCHMOD CACHE  INTERNAL "")
SET(HAVE_FCNTL CACHE  INTERNAL "")
SET(HAVE_FDATASYNC CACHE  INTERNAL "")
SET(HAVE_DECL_FDATASYNC CACHE INTERNAL "")
SET(HAVE_FEDISABLEEXCEPT CACHE  INTERNAL "")
SET(HAVE_FSEEKO CACHE  INTERNAL "")
SET(HAVE_FSYNC CACHE  INTERNAL "")
SET(HAVE_GETHOSTBYADDR_R CACHE  INTERNAL "")
SET(HAVE_GETHRTIME CACHE  INTERNAL "")
# Check needed HAVE_GETNAMEINFO
SET(HAVE_GETPASS CACHE  INTERNAL "")
SET(HAVE_GETPASSPHRASE CACHE  INTERNAL "")
SET(HAVE_GETPWNAM CACHE  INTERNAL "")
SET(HAVE_GETPWUID CACHE  INTERNAL "")
SET(HAVE_GETRLIMIT CACHE  INTERNAL "")
SET(HAVE_GETRUSAGE CACHE  INTERNAL "")
SET(HAVE_INITGROUPS CACHE  INTERNAL "")
SET(HAVE_ISSETUGID CACHE  INTERNAL "")
SET(HAVE_GETUID CACHE  INTERNAL "")
SET(HAVE_GETEUID CACHE  INTERNAL "")
SET(HAVE_GETGID CACHE  INTERNAL "")
SET(HAVE_GETEGID CACHE  INTERNAL "")
SET(HAVE_LSTAT CACHE  INTERNAL "")
SET(HAVE_MADVISE CACHE  INTERNAL "")
SET(HAVE_MALLOC_INFO CACHE  INTERNAL "")
SET(HAVE_MEMRCHR CACHE  INTERNAL "")
SET(HAVE_MLOCK CACHE  INTERNAL "")
SET(HAVE_MLOCKALL CACHE  INTERNAL "")
SET(HAVE_MMAP CACHE  INTERNAL "")
SET(HAVE_MMAP64 CACHE  INTERNAL "")
SET(HAVE_POLL CACHE INTERNAL "")
SET(HAVE_POSIX_FALLOCATE CACHE  INTERNAL "")
SET(HAVE_POSIX_MEMALIGN CACHE  INTERNAL "")
SET(HAVE_PREAD CACHE  INTERNAL "")
SET(HAVE_PTHREAD_CONDATTR_SETCLOCK CACHE  INTERNAL "")
SET(HAVE_PTHREAD_SIGMASK CACHE  INTERNAL "")
SET(HAVE_READDIR_R CACHE  INTERNAL "")
SET(HAVE_READLINK CACHE  INTERNAL "")
SET(HAVE_REALPATH CACHE  INTERNAL "")
SET(HAVE_SCHED_YIELD CACHE  INTERNAL "")
SET(HAVE_SETFD CACHE  INTERNAL "")
SET(HAVE_SIGACTION CACHE  INTERNAL "")
SET(HAVE_SLEEP CACHE  INTERNAL "")
SET(HAVE_STPCPY CACHE  INTERNAL "")
SET(HAVE_STPNCPY CACHE  INTERNAL "")
SET(HAVE_STRLCPY CACHE  INTERNAL "")
SET(HAVE_STRNLEN 1 CACHE  INTERNAL "")
SET(HAVE_STRLCAT CACHE  INTERNAL "")
SET(HAVE_STRSIGNAL CACHE  INTERNAL "")
SET(HAVE_FGETLN CACHE  INTERNAL "")
SET(HAVE_STRSEP CACHE  INTERNAL "")
SET(HAVE_TELL 1 CACHE  INTERNAL "")
SET(HAVE_VASPRINTF CACHE  INTERNAL "")
SET(HAVE_MEMALIGN CACHE  INTERNAL "")
SET(HAVE_NL_LANGINFO CACHE  INTERNAL "")
SET(HAVE_HTONLL CACHE  INTERNAL "")
SET(DNS_USE_CPU_CLOCK_FOR_ID CACHE  INTERNAL "")
SET(HAVE_EPOLL CACHE  INTERNAL "")
# Disabled in configure.cmake HAVE_EVENT_PORTS
# Check needed HAVE_INET_NTOP
SET(HAVE_WORKING_KQUEUE CACHE  INTERNAL "")
SET(HAVE_TIMERADD CACHE  INTERNAL "")
SET(HAVE_TIMERCLEAR CACHE  INTERNAL "")
SET(HAVE_TIMERCMP CACHE  INTERNAL "")
SET(HAVE_TIMERISSET CACHE  INTERNAL "")

# WL2373
SET(HAVE_SYS_TIME_H CACHE  INTERNAL "")
SET(HAVE_SYS_TIMES_H CACHE  INTERNAL "")
SET(HAVE_TIMES CACHE  INTERNAL "")
SET(HAVE_GETTIMEOFDAY CACHE  INTERNAL "")

# Symbols
SET(HAVE_LRAND48 CACHE  INTERNAL "")
SET(GWINSZ_IN_SYS_IOCTL CACHE INTERNAL "")
SET(FIONREAD_IN_SYS_IOCTL CACHE INTERNAL "")
SET(FIONREAD_IN_SYS_FILIO CACHE INTERNAL "")
SET(HAVE_SIGEV_THREAD_ID CACHE INTERNAL "")
SET(HAVE_SIGEV_PORT CACHE INTERNAL "")
# Check needed HAVE_LOG2

# Check needed HAVE_ISINF

SET(HAVE_TIMER_CREATE CACHE INTERNAL "")   # Only needed by CMake
SET(HAVE_TIMER_SETTIME CACHE INTERNAL "")  # Only needed by CMake
SET(HAVE_KQUEUE CACHE INTERNAL "")         # Only needed by CMake
SET(HAVE_EVFILT_TIMER CACHE INTERNAL "")   # Only needed by CMake
# Derived result HAVE_KQUEUE_TIMERS
# Derived result HAVE_POSIX_TIMERS
# Derived result HAVE_WINDOWS_TIMERS
# Derived result HAVE_MY_TIMER

# Endianess
SET(HAVE_WORDS_BIGENDIAN TRUE CACHE  INTERNAL "")  # Only needed by CMake
SET(WORDS_BIGENDIAN CACHE  INTERNAL "")

# Type sizes
SET(HAVE_SIZEOF_SIGSET_T FALSE CACHE  INTERNAL "")
SET(HAVE_SIZEOF_MODE_T FALSE CACHE  INTERNAL "")
# Check needed VOIDP
SET(HAVE_SIZEOF_CHARP TRUE CACHE  INTERNAL "")
SET(SIZEOF_CHARP ${CMAKE_SIZEOF_VOID_P} CACHE  INTERNAL "")
SET(HAVE_SIZEOF_LONG TRUE CACHE  INTERNAL "")
SET(SIZEOF_LONG 4 CACHE  INTERNAL "")
SET(HAVE_SIZEOF_CHAR TRUE CACHE  INTERNAL "")
SET(SIZEOF_CHAR 1 CACHE  INTERNAL "")
SET(HAVE_SIZEOF_SHORT TRUE CACHE  INTERNAL "")
SET(SIZEOF_SHORT 2 CACHE  INTERNAL "")
SET(HAVE_SIZEOF_INT TRUE CACHE  INTERNAL "")
SET(SIZEOF_INT 4 CACHE  INTERNAL "")
SET(HAVE_SIZEOF_LONG_LONG TRUE CACHE  INTERNAL "")
SET(SIZEOF_LONG_LONG 8 CACHE  INTERNAL "")
SET(HAVE_SIZEOF_OFF_T TRUE CACHE  INTERNAL "")
SET(SIZEOF_OFF_T 4 CACHE  INTERNAL "")
SET(HAVE_SIZEOF_UINT FALSE CACHE  INTERNAL "")
SET(HAVE_SIZEOF_ULONG FALSE CACHE  INTERNAL "")
SET(HAVE_SIZEOF_U_INT32_T FALSE CACHE  INTERNAL "")

# Code tests
# Hardcoded in configure.cmake SOCKET_SIZE_TYPE
SET(HAVE_PTHREAD_YIELD_ZERO_ARG CACHE  INTERNAL "")
SET(STACK_DIRECTION -1 CACHE INTERNAL "")
SET(TIME_WITH_SYS_TIME CACHE INTERNAL "")
SET(HAVE_FCNTL_NONBLOCK CACHE  INTERNAL "") # Only needed by CMake
# Derived result NO_FCNTL_NONBLOCK
SET(C_HAS_inline CACHE INTERNAL "")
SET(C_HAS___inline 1 CACHE INTERNAL "")
# Not checked for Windows HAVE_PAUSE_INSTRUCTION
# Not checked for Windows HAVE_FAKE_PAUSE_INSTRUCTION
# Not checked for Windows HAVE_ABI_CXA_DEMANGLE
SET(HAVE_BSS_START CACHE  INTERNAL "")
SET(HAVE_BUILTIN_UNREACHABLE_C CACHE  INTERNAL "")   # Only needed by CMake
SET(HAVE_BUILTIN_UNREACHABLE_CXX CACHE  INTERNAL "") # Only needed by CMake
# Derived result HAVE_BUILTIN_UNREACHABLE
SET(HAVE_BUILTIN_EXPECT CACHE  INTERNAL "")
SET(HAVE_BUILTIN_STPCPY CACHE  INTERNAL "")
SET(HAVE_GCC_ATOMIC_BUILTINS CACHE  INTERNAL "")
# Derived result HAVE_VALGRIND

# IPV6
SET(HAVE_NETINET_IN6_H CACHE  INTERNAL "")
# Check needed HAVE_STRUCT_SOCKADDR_IN6
# Check needed HAVE_STRUCT_IN6_ADDR
SET(HAVE_IPV6 TRUE CACHE  INTERNAL "")

SET(HAVE_SOCKADDR_STORAGE_SS_FAMILY 1 CACHE  INTERNAL "")
SET(HAVE_SOCKADDR_IN_SIN_LEN CACHE INTERNAL "")
SET(HAVE_SOCKADDR_IN6_SIN6_LEN CACHE INTERNAL "")

#
# Platform specific CMake files
#
# Windows stuff, mostly functions, that have Posix analogs but named differently
SET(HAVE_IPPROTO_IPV6 CACHE  INTERNAL "")
SET(HAVE_IPV6_V6ONLY 1 CACHE  INTERNAL "")
SET(HAVE_S_IROTH CACHE INTERNAL "")
SET(HAVE__S_IREAD 1 CACHE  INTERNAL "")
SET(HAVE_S_IFIFO CACHE INTERNAL "")
SET(HAVE__S_IFIFO 1 CACHE  INTERNAL "")
SET(HAVE_SIGQUIT CACHE  INTERNAL "")
SET(HAVE_SIGTERM 1 CACHE  INTERNAL "")
SET(HAVE_SIGPIPE CACHE  INTERNAL "")
SET(HAVE_SIGINT 1 CACHE  INTERNAL "")
SET(HAVE__isnan 1 CACHE  INTERNAL "")
SET(HAVE_SIZE_OF_SSIZE_T FALSE CACHE  INTERNAL "") # Only needed by CMake
# Derived result ssize_t
# Hardcoded in Windows.cmake FN_NO_CASE_SENSE

#
# Innodb.cmake
#
SET(HAVE_SCHED_GETCPU CACHE  INTERNAL "")
SET(HAVE_NANOSLEEP CACHE  INTERNAL "")
SET(HAVE_ASPRINTF CACHE INTERNAL "")

#
# Auth
#
SET(HAVE_PEERCRED CACHE INTERNAL "")

#
# PAM
#
SET(HAVE_PAM_APPL_H CACHE  INTERNAL "")
SET(HAVE_PAM_PAM_APPL_H CACHE  INTERNAL "")
SET(HAVE_PAM_START_IN_PAM CACHE  INTERNAL "")
SET(HAVE_GETPWNAM_R CACHE  INTERNAL "")
SET(HAVE_GETGRNAM_R CACHE  INTERNAL "")

#
# Maintainer.cmake.
#
SET(HAVE_WVLA CACHE INTERNAL "")

# Seems like a bug?
SET(HAVE_ACCESS 1 CACHE  INTERNAL "")
