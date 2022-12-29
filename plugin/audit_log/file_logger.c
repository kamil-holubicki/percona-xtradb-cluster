/* Copyright (C) 2012 Monty Program Ab

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA */

#include <my_global.h>
#include <mysql/plugin.h>
#include <my_sys.h>
#include <string.h>
#include <my_thread_local.h>

#include "logger.h"
#include "audit_log.h"

extern char *mysql_data_home;

#ifndef FLOGGER_NO_PSI
  #define flogger_mutex_init(A,B,C) \
            if ((B)->thread_safe) \
              mysql_mutex_init(A,&((B)->lock),C)

  #define flogger_mutex_destroy(A) \
            if ((A)->thread_safe) \
              mysql_mutex_destroy(&((A)->lock))

  #define flogger_mutex_lock(A) \
            if ((A)->thread_safe) \
              mysql_mutex_lock(&((A)->lock))

  #define flogger_mutex_unlock(A) \
            if ((A)->thread_safe) \
              mysql_mutex_unlock(&((A)->lock))
#else
  #define flogger_mutex_init(A,B,C) \
            if ((B)->thread_safe) \
              pthread_mutex_init(&((B)->lock.m_mutex), C)

  #define flogger_mutex_destroy(A) \
            if ((A)->thread_safe) \
              pthread_mutex_destroy(&((A)->lock.m_mutex))

  #define flogger_mutex_lock(A) \
            if ((A)->thread_safe) \
              pthread_mutex_lock(&((A)->lock.m_mutex))

  #define flogger_mutex_unlock(A) \
            if ((A)->thread_safe) \
              pthread_mutex_unlock(&((A)->lock.m_mutex))
#endif /*!FLOGGER_NO_PSI*/

#if defined(HAVE_PSI_INTERFACE) && !defined(FLOGGER_NO_PSI)
/* These belong to the service initialization */
static PSI_mutex_key key_LOCK_logger_service;
static PSI_mutex_info mutex_list[]=
{{ &key_LOCK_logger_service, "file_logger::lock", PSI_FLAG_GLOBAL}};
#else
#define key_LOCK_logger_service 0
#endif /*HAVE_PSI_INTERFACE && !FLOGGER_NO_PSI*/

typedef struct logger_handle_st {
  File file;
  char path[FN_REFLEN];
  unsigned long long size_limit;
  unsigned int rotations;
  size_t path_len;
  mysql_mutex_t lock;
  int thread_safe;
} LSFS;


#define LOG_FLAGS (O_APPEND | O_CREAT | O_WRONLY)

static unsigned int n_dig(unsigned int i)
{
  return (i == 0) ? 0 : ((i < 10) ? 1 : ((i < 100) ? 2 : 3));
}

LOGGER_HANDLE *logger_open(const char *path,
                           unsigned long long size_limit,
                           unsigned int rotations,
                           int thread_safe,
                           logger_prolog_func_t header)
{
  LOGGER_HANDLE new_log, *l_perm;
  MY_STAT stat_arg;
  char buf[128];
  size_t len;

  /*
    I don't think we ever need more rotations,
    but if it's so, the rotation procedure should be adapted to it.
  */
  if (rotations > 999)
    return 0;

  new_log.rotations= rotations;
  new_log.size_limit= size_limit;
  new_log.path_len= strlen(fn_format(new_log.path, path,
        mysql_data_home, "", MY_UNPACK_FILENAME));
  new_log.thread_safe= thread_safe;

  if (new_log.path_len+n_dig(rotations)+1 > FN_REFLEN)
  {
    errno= ENAMETOOLONG;
    /* File path too long */
    return 0;
  }

  if ((new_log.file= my_open(new_log.path, LOG_FLAGS, 0666)) < 0)
  {
    errno= my_errno();
    /* Check errno for the cause */
    return 0;
  }

  if (my_fstat(new_log.file, &stat_arg, MYF(0)))
  {
    errno= my_errno();
    my_close(new_log.file, MYF(0));
    new_log.file= -1;
    return 0;
  }

  if (!(l_perm= (LOGGER_HANDLE *) my_malloc(key_memory_audit_log_logger_handle,
                                            sizeof(LOGGER_HANDLE), MYF(0))))
  {
    my_close(new_log.file, MYF(0));
    new_log.file= -1;
    return 0; /* End of memory */
  }
  *l_perm= new_log;

  flogger_mutex_init(key_LOCK_logger_service, l_perm,
                     MY_MUTEX_INIT_FAST);

  len= header(&stat_arg, buf, sizeof(buf));
  my_write(l_perm->file, (uchar *)buf, len, MYF(0));

  return l_perm;
}

int logger_close(LOGGER_HANDLE *log, logger_epilog_func_t footer)
{
  int result;
  File file= log->file;
  char buf[128];
  size_t len;

  len= footer(buf, sizeof(buf));
  my_write(file, (uchar *)buf, len, MYF(0));

  flogger_mutex_destroy(log);
  my_free(log);
  if ((result= my_close(file, MYF(0))))
    errno= my_errno();
  return result;
}


int logger_reopen(LOGGER_HANDLE *log, logger_prolog_func_t header,
                  logger_epilog_func_t footer)
{
  int result= 0;
  MY_STAT stat_arg;
  char buf[128];
  size_t len;

  flogger_mutex_lock(log);

  len= footer(buf, sizeof(buf));
  my_write(log->file, (uchar *)buf, len, MYF(0));

  if ((result= my_close(log->file, MYF(0))))
  {
    errno= my_errno();
    goto error;
  }

  if ((log->file= my_open(log->path, LOG_FLAGS, MYF(0))) < 0)
  {
    errno= my_errno();
    result= 1;
    goto error;
  }

  if ((result= my_fstat(log->file, &stat_arg, MYF(0))))
  {
    errno= my_errno();
    goto error;
  }

  len= header(&stat_arg, buf, sizeof(buf));
  my_write(log->file, (uchar *)buf, len, MYF(0));

error:
  flogger_mutex_unlock(log);

  return result;
}


static char *logname(LOGGER_HANDLE *log, char *buf, size_t buf_len,
                     unsigned int n_log)
{
  snprintf(buf+log->path_len, buf_len, ".%0*u", n_dig(log->rotations),
           n_log);
  return buf;
}


static int do_rotate(LOGGER_HANDLE *log) {
  char new_name_buf[FN_REFLEN] = {0};
  char old_name_buf[FN_REFLEN] = {0};
  char *new_name = NULL;
  char *old_name = NULL;
  unsigned int i = 0;

  if (log->rotations == 0) return 0;

  memcpy(new_name_buf, log->path, log->path_len);
  memcpy(old_name_buf, log->path, log->path_len);

  for (i = log->rotations; i > 0; i--) {
    new_name = logname(log, new_name_buf, sizeof(new_name_buf), i);

    if (i > 1) {
      old_name = logname(log, old_name_buf, sizeof(new_name_buf), i - 1);
    }
    else {
      old_name = old_name_buf;
      old_name_buf[log->path_len] = 0;
    }

    if (0 == access(old_name, F_OK) &&
        0 != my_rename(old_name, new_name, MYF(0))) {
      errno = my_errno();
      return -1;
    }
  }

  if (0 != my_close(log->file, MYF(0))) {
    errno = my_errno();
    return -1;
  }

  log->file = my_open(log->path, LOG_FLAGS, MYF(0));

  errno = my_errno();
  return log->file < 0 ? -1 : 0;
}


int logger_write(LOGGER_HANDLE *log, const char *buffer, size_t size,
                 log_record_state_t state)
{
  int result;
  my_off_t filesize;

  flogger_mutex_lock(log);

  result= my_write(log->file, (uchar *) buffer, size, MYF(0));

  if (state == LOG_RECORD_COMPLETE && log->rotations > 0)
  {
    if ((filesize= my_tell(log->file, MYF(0))) == (my_off_t) -1 ||
        ((unsigned long long)filesize >= log->size_limit &&
         do_rotate(log)))
    {
      result= -1;
      errno= my_errno();
      goto exit; /* Log rotation needed but failed */
    }
  }

exit:
  flogger_mutex_unlock(log);
  return result;
}


void logger_init_mutexes()
{
#if defined(HAVE_PSI_INTERFACE) && !defined(FLOGGER_NO_PSI) && !defined(FLOGGER_NO_THREADSAFE)
  mysql_mutex_register(AUDIT_LOG_PSI_CATEGORY,
                       mutex_list, array_elements(mutex_list));
#endif /*HAVE_PSI_INTERFACE && !FLOGGER_NO_PSI*/
}

int logger_sync(LOGGER_HANDLE *log)
{
  return my_sync(log->file, MYF(0));
}

void logger_set_size_limit(LOGGER_HANDLE *log, unsigned long long size_limit)
{
  log->size_limit= size_limit;
}

void logger_set_rotations(LOGGER_HANDLE *log, unsigned int rotations)
{
  log->rotations= rotations;
}
