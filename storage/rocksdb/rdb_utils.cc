/*
   Copyright (c) 2016, Facebook, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/* This C++ file's header */
#include "./rdb_utils.h"

/* C++ standard header files */
#include <array>
#include <sstream>
#include <string>
#include <vector>

/* C standard header files */
#include <ctype.h>

/* MySQL header files */
#include "my_dir.h"

/* MyRocks header files */
#include "./ha_rocksdb.h"

namespace myrocks {

/*
  Skip past any spaces in the input
*/
const char *rdb_skip_spaces(const struct charset_info_st *const cs,
                            const char *str) {
  DBUG_ASSERT(cs != nullptr);
  DBUG_ASSERT(str != nullptr);

  while (my_isspace(cs, *str)) {
    str++;
  }

  return str;
}

/*
  Compare (ignoring case) to see if str2 is the next data in str1.
  Note that str1 can be longer but we only compare up to the number
  of characters in str2.
*/
bool rdb_compare_strings_ic(const char *const str1, const char *const str2) {
  DBUG_ASSERT(str1 != nullptr);
  DBUG_ASSERT(str2 != nullptr);

  // Scan through the strings
  size_t ii;
  for (ii = 0; str2[ii]; ii++) {
    if (toupper(static_cast<int>(str1[ii])) !=
        toupper(static_cast<int>(str2[ii]))) {
      return false;
    }
  }

  return true;
}

/*
  Scan through an input string looking for pattern, ignoring case
  and skipping all data enclosed in quotes.
*/
const char *rdb_find_in_string(const char *str, const char *pattern,
                               bool *const succeeded) {
  char quote = '\0';
  bool escape = false;

  DBUG_ASSERT(str != nullptr);
  DBUG_ASSERT(pattern != nullptr);
  DBUG_ASSERT(succeeded != nullptr);

  *succeeded = false;

  for (; *str; str++) {
    /* If we found a our starting quote character */
    if (*str == quote) {
      /* If it was escaped ignore it */
      if (escape) {
        escape = false;
      }
      /* Otherwise we are now outside of the quoted string */
      else {
        quote = '\0';
      }
    }
    /* Else if we are currently inside a quoted string? */
    else if (quote != '\0') {
      /* If so, check for the escape character */
      escape = !escape && *str == '\\';
    }
    /* Else if we found a quote we are starting a quoted string */
    else if (*str == '"' || *str == '\'' || *str == '`') {
      quote = *str;
    }
    /* Else we are outside of a quoted string - look for our pattern */
    else {
      if (rdb_compare_strings_ic(str, pattern)) {
        *succeeded = true;
        return str;
      }
    }
  }

  // Return the character after the found pattern or the null terminateor
  // if the pattern wasn't found.
  return str;
}

/*
  See if the next valid token matches the specified string
*/
const char *rdb_check_next_token(const struct charset_info_st *const cs,
                                 const char *str, const char *const pattern,
                                 bool *const succeeded) {
  DBUG_ASSERT(cs != nullptr);
  DBUG_ASSERT(str != nullptr);
  DBUG_ASSERT(pattern != nullptr);
  DBUG_ASSERT(succeeded != nullptr);

  // Move past any spaces
  str = rdb_skip_spaces(cs, str);

  // See if the next characters match the pattern
  if (rdb_compare_strings_ic(str, pattern)) {
    *succeeded = true;
    return str + strlen(pattern);
  }

  *succeeded = false;
  return str;
}

/*
  Parse id
*/
const char *rdb_parse_id(const struct charset_info_st *const cs,
                         const char *str, std::string *const id) {
  DBUG_ASSERT(cs != nullptr);
  DBUG_ASSERT(str != nullptr);

  // Move past any spaces
  str = rdb_skip_spaces(cs, str);

  if (*str == '\0') {
    return str;
  }

  char quote = '\0';
  if (*str == '`' || *str == '"') {
    quote = *str++;
  }

  size_t len = 0;
  const char *start = str;

  if (quote != '\0') {
    for (;;) {
      if (*str == '\0') {
        return str;
      }

      if (*str == quote) {
        str++;
        if (*str != quote) {
          break;
        }
      }

      str++;
      len++;
    }
  } else {
    while (!my_isspace(cs, *str) && *str != '(' && *str != ')' && *str != '.' &&
           *str != ',' && *str != '\0') {
      str++;
      len++;
    }
  }

  // If the user requested the id create it and return it
  if (id != nullptr) {
    *id = std::string("");
    id->reserve(len);
    while (len--) {
      *id += *start;
      if (*start++ == quote) {
        start++;
      }
    }
  }

  return str;
}

/*
  Skip id
*/
const char *rdb_skip_id(const struct charset_info_st *const cs,
                        const char *str) {
  DBUG_ASSERT(cs != nullptr);
  DBUG_ASSERT(str != nullptr);

  return rdb_parse_id(cs, str, nullptr);
}

/*
  Parses a given string into tokens (if any) separated by a specific delimiter.
*/
const std::vector<std::string> parse_into_tokens(const std::string &s,
                                                 const char delim) {
  std::vector<std::string> tokens;
  std::string t;
  std::stringstream ss(s);

  while (getline(ss, t, delim)) {
    tokens.push_back(t);
  }

  return tokens;
}

static const std::size_t rdb_hex_bytes_per_char = 2;
static const std::array<char, 16> rdb_hexdigit = {{'0', '1', '2', '3', '4', '5',
                                                   '6', '7', '8', '9', 'a', 'b',
                                                   'c', 'd', 'e', 'f'}};

/*
  Convert data into a hex string with optional maximum length.
  If the data is larger than the maximum length trancate it and append "..".
*/
std::string rdb_hexdump(const char *data, const std::size_t data_len,
                        const std::size_t maxsize) {
  DBUG_ASSERT(data != nullptr);

  // Count the elements in the string
  std::size_t elems = data_len;
  // Calculate the amount of output needed
  std::size_t len = elems * rdb_hex_bytes_per_char;
  std::string str;

  if (maxsize != 0 && len > maxsize) {
    // If the amount of output is too large adjust the settings
    // and leave room for the ".." at the end
    elems = (maxsize - 2) / rdb_hex_bytes_per_char;
    len = elems * rdb_hex_bytes_per_char + 2;
  }

  // Reserve sufficient space to avoid reallocations
  str.reserve(len);

  // Loop through the input data and build the output string
  for (std::size_t ii = 0; ii < elems; ii++, data++) {
    uint8_t ch = (uint8_t)*data;
    str += rdb_hexdigit[ch >> 4];
    str += rdb_hexdigit[ch & 0x0F];
  }

  // If we can't fit it all add the ".."
  if (elems != data_len) {
    str += "..";
  }

  return str;
}

/*
  Attempt to access the database subdirectory to see if it exists
*/
bool rdb_database_exists(const std::string &db_name) {
  const std::string dir =
      std::string(mysql_real_data_home) + FN_DIRSEP + db_name;
  struct st_my_dir *const dir_info =
      my_dir(dir.c_str(), MYF(MY_DONT_SORT | MY_WANT_STAT));
  if (dir_info == nullptr) {
    return false;
  }

  my_dirend(dir_info);
  return true;
}

void rdb_log_status_error(const rocksdb::Status &s, const char *msg) {
  if (msg == nullptr) {
    // NO_LINT_DEBUG
    sql_print_error("RocksDB: status error, code: %d, error message: %s",
                    s.code(), s.ToString().c_str());
    return;
  }

  // NO_LINT_DEBUG
  sql_print_error("RocksDB: %s, Status Code: %d, Status: %s", msg, s.code(),
                  s.ToString().c_str());
}

void warn_about_bad_patterns(const Regex &regex, const char *name) {
  // There was some invalid regular expression data in the patterns supplied

  // NO_LINT_DEBUG
  sql_print_warning("RocksDB: Invalid pattern in %s: %s", name,
                    regex.pattern().c_str());
}

// Split a string based on a delimiter.  Two delimiters in a row will not add
// an empty string in the set.
std::vector<std::string> split_into_vector(const std::string &input,
                                           char delimiter) {
  size_t pos;
  size_t start = 0;
  std::vector<std::string> elems;

  // Find next delimiter
  while ((pos = input.find(delimiter, start)) != std::string::npos) {
    // If there is any data since the last delimiter add it to the list
    if (pos > start) elems.push_back(input.substr(start, pos - start));

    // Set our start position to the character after the delimiter
    start = pos + 1;
  }

  // Add a possible string since the last delimiter
  if (input.length() > start) elems.push_back(input.substr(start));

  // Return the resulting list back to the caller
  return elems;
}

bool rdb_check_rocksdb_corruption() {
  return !my_access(myrocks::rdb_corruption_marker_file_name().c_str(), F_OK);
}

void rdb_persist_corruption_marker() {
  const std::string &fileName = myrocks::rdb_corruption_marker_file_name();
  int fd = my_open(fileName.c_str(), O_CREAT | O_SYNC, MYF(MY_WME));
  if (fd < 0) {
    // NO_LINT_DEBUG
    sql_print_error(
        "RocksDB: Can't create file %s to mark rocksdb as "
        "corrupted.",
        fileName.c_str());
  } else {
    // NO_LINT_DEBUG
    sql_print_information(
        "RocksDB: Creating the file %s to abort mysqld "
        "restarts. Remove this file from the data directory "
        "after fixing the corruption to recover. ",
        fileName.c_str());
  }

  int ret = my_close(fd, MYF(MY_WME));
  if (ret) {
    sql_print_error("RocksDB: Error (%d) closing the file %s", ret,
                    fileName.c_str());
  }
}

}  // namespace myrocks
