/* Copyright (c) 2016, 2023, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/*
  This operates on the error messages that will be included into the
  client library (libmysql), NOT the messages that the server sends
  to clients!
*/

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

#include "errmsg.h"
#include "m_string.h"
#include "my_compiler.h"
#include "my_getopt.h"
#include "my_sys.h"
#include "print_version.h"
#include "welcome_copyright_notice.h"

static const char *INFILE = "errmsg.h";
static const char *OUTFILE = "mysqlclient_ername.h";

static struct my_option my_long_options[] = {
    {"in_file", 'F', "Input file", &INFILE, &INFILE, nullptr, GET_STR,
     REQUIRED_ARG, 0, 0, 0, nullptr, 0, nullptr},
    {"out_file", 'O', "Output filename (mysqlclient_ername.h)", &OUTFILE,
     &OUTFILE, nullptr, GET_STR, REQUIRED_ARG, 0, 0, 0, nullptr, 0, nullptr},
    {nullptr, 0, nullptr, nullptr, nullptr, nullptr, GET_NO_ARG, NO_ARG, 0, 0,
     0, nullptr, 0, nullptr}};

static void usage() {
  print_version();
  puts(ORACLE_WELCOME_COPYRIGHT_NOTICE("2016"));
  printf(
      "Usage: ./comp_client_err --in_file=name --out_file=name\n"
      "Options: \n"
      " -F, --in_file=name  Input file name\n"
      " -O, --out_file=name Output file name\n");
}

static bool get_one_option(int, const struct my_option *, char *) {
  return false;
}

static int get_options(int *argc, char ***argv) {
  if (*argc == 1 || *argc == 2 || *argc > 3) {
    usage();
    return 1;
  }

  const int ho_error =
      handle_options(argc, argv, my_long_options, get_one_option);
  return ho_error;
}

int reject_lines(const char *str) {
  if (is_prefix(str, "ERRMSG_INCLUDED") || is_prefix(str, "CR_MIN_ERROR") ||
      is_prefix(str, "CR_MAX_ERROR") || is_prefix(str, "CLIENT_ERRMAP") ||
      is_prefix(str, "CR_ERROR_FIRST") || is_prefix(str, "CR_ERROR_LAST"))
    return 1;
  return 0;
}

int main(int argc, char *argv[]) {
  MY_INIT(argv[0]);

  if (get_options(&argc, &argv)) return 1;

  FILE *infile;
  if (!(infile = my_fopen(INFILE, O_RDONLY, MYF(MY_WME)))) return 1;

  FILE *outfile;
  if (!(outfile = my_fopen(OUTFILE, O_WRONLY, MYF(MY_WME)))) return 1;

  fprintf(outfile, ORACLE_GPL_COPYRIGHT_NOTICE("2016"));
  fprintf(outfile, "/* Autogenerated file, please don't edit */\n\n");

  char buf[512];
  char *str;
  while ((str = fgets(buf, sizeof(buf), infile))) {
    /*
      Lines containing error name and error code start with
      "#define" keyword.
    */
    if (is_prefix(str, "#define")) {
      /*
        Ignoring the "#define" string present at the beginning
        of each line.
      */
      while (*str != ' ') str++;
      str++;

      /* Ignoring the lines which don't require any parsing */
      if (reject_lines(str)) continue;

      uint count = 0;
      char *last_token;
      for (char *err = my_strtok_r(str, " \n\t", &last_token); err != nullptr;
           err = my_strtok_r(nullptr, " \n\t", &last_token)) {
        if (count == 0)
          fprintf(outfile, "{ \"%s\", ", err);
        else if (count == 1) {
          long err_code;
          if (!str2int(err, 10, (long)0, (long)65536, &err_code)) return 1;

          fprintf(outfile, "%s, ", err);
          fprintf(outfile, "\"%s\", 0, 0, 0},\n", ER_CLIENT(err_code));
        }

        count++;
        count = count % 2;
      }
    }
  }

  my_fclose(infile, MYF(0));
  my_fclose(outfile, MYF(0));

  return 0;
}
