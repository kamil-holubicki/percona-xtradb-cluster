/* Copyright (c) 2018 Percona LLC and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 of
   the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

#include "system_key.h"
#include <stdlib.h>
#include "my_sys.h"

struct Valid_percona_system_key
{
  const char *key_name;
  size_t key_length;
  my_bool is_prefix; // If set valid percona key must start with prefix key_name. If is_prefix is not set
                     // valid key must match exactly the name from key_name
};

/** 
   System keys cannot have ':' in their name. We use ':' as a separator between
   system key's name and system key's version.
*/
struct Valid_percona_system_key valid_percona_system_keys[] = { {PERCONA_BINLOG_KEY_NAME, 16, FALSE},
                                                                {PERCONA_INNODB_KEY_NAME, 32, TRUE},
								{PERCONA_REDO_KEY_NAME, 32, FALSE}};
const size_t valid_percona_system_keys_size = array_elements(valid_percona_system_keys);

my_bool is_valid_percona_system_key(const char *key_name, size_t *key_length)
{
  uint i= 0;
  for(; i < valid_percona_system_keys_size; ++i)
  {
    if ((valid_percona_system_keys[i].is_prefix && strstr(key_name, valid_percona_system_keys[i].key_name) == key_name) ||
        (!valid_percona_system_keys[i].is_prefix && strcmp(valid_percona_system_keys[i].key_name, key_name) == 0))
    {
      *key_length = valid_percona_system_keys[i].key_length;
      return TRUE;
    }
  }
  return FALSE;
}

// Only parse the latest key - from system_keys_container - do not parse keys from keys_container
uchar* parse_system_key(const uchar *key, const size_t key_length, uint *key_version,
                        uchar **key_data, size_t *key_data_length)
{
  size_t key_version_length= 0;
  ulong ulong_key_version= 0;
  char *version= NULL, *endptr= NULL;

  if (key == NULL || key_length == 0)
    return NULL;

  for (; key[key_version_length] != ':' && key_version_length < key_length; ++key_version_length);
  if (key_version_length == 0 || key_version_length == key_length)
    return NULL; // no version found

  version= (char*)(my_malloc(PSI_NOT_INSTRUMENTED, key_version_length+1, MYF(0)));
  if (version == NULL)
    return NULL;

  memcpy(version, key, key_version_length);
  version[key_version_length]= '\0';
  endptr= version;

  ulong_key_version= strtoul(version, &endptr, 10);
  if (ulong_key_version > UINT_MAX || *endptr != '\0')
  {
    my_free(version);
    return NULL; // conversion failed
  }
  my_free(version);

  assert(ulong_key_version <= UINT_MAX); // sanity check

  *key_data_length= key_length - (key_version_length + 1); // skip ':' after key version
  if (*key_data_length == 0)
    return NULL;
  assert(*key_data_length <= 512);

  *key_data= (uchar*)(my_malloc(PSI_NOT_INSTRUMENTED, sizeof(uchar)*(*key_data_length), MYF(0)));
  if (*key_data == NULL)
    return NULL;

  memcpy(*key_data, key+key_version_length+1, *key_data_length); // skip ':' after key version
  *key_version= (uint)ulong_key_version;
  return *key_data;
}
