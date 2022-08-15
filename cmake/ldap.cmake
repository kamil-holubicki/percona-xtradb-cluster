# Copyright (c) 2019, 2022, Oracle and/or its affiliates.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have included with MySQL.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

# cmake -DWITH_LDAP=system|</path/to/custom/installation>
# system is the default

INCLUDE (CheckIncludeFile)
INCLUDE (CheckIncludeFiles)

SET(WITH_LDAP_DOC "\nsystem (use the OS ldap library)")
STRING_APPEND(WITH_LDAP_DOC ", \n</path/to/custom/installation>")

STRING(REPLACE "\n" "| " WITH_LDAP_DOC_STRING "${WITH_LDAP_DOC}")

MACRO(FIND_SYSTEM_LDAP)
  IF(WIN32)
    SET(LDAP_SYSTEM_LIBRARY Wldap32 CACHE INTERNAL
      "LDAP library is /c/Windows/system32/Wldap32.dll"
      )
  ELSE()
    FIND_LIBRARY(LDAP_SYSTEM_LIBRARY
      NAMES ldap_r ldap
      )
    FIND_LIBRARY(LBER_SYSTEM_LIBRARY
      NAMES lber
      )
  ENDIF()
  IF(LDAP_SYSTEM_LIBRARY AND (WIN32 OR LBER_SYSTEM_LIBRARY))
    SET(LDAP_LIBRARY ${LDAP_SYSTEM_LIBRARY})
    SET(LBER_LIBRARY ${LBER_SYSTEM_LIBRARY})
    MESSAGE(STATUS "LBER_LIBRARY ${LBER_LIBRARY}")
    MESSAGE(STATUS "LDAP_LIBRARY ${LDAP_LIBRARY}")
  ENDIF()

  IF(WIN32)
    # LDAP system header is Winldap.h and is in some Windows SDK
  ELSE()
    CMAKE_PUSH_CHECK_STATE()

    # For Solaris 11.3 we need to explicitly search here:
    IF(SOLARIS)
      INCLUDE_DIRECTORIES(BEFORE SYSTEM /usr/include/openldap)
      SET(CMAKE_REQUIRED_INCLUDES "/usr/include/openldap")
    ENDIF()

    CHECK_INCLUDE_FILE(lber.h HAVE_LBER_H)
    CHECK_INCLUDE_FILE(ldap.h HAVE_LDAP_H)
    CMAKE_POP_CHECK_STATE()
  ENDIF()

ENDMACRO()

MACRO(FIND_CUSTOM_LDAP)
  FIND_PATH(LDAP_INCLUDE_DIR
    NAMES ldap.h
    PATHS ${WITH_LDAP_PATH}/include
    NO_DEFAULT_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    )

  # On mac this list is <.dylib;.so;.a>
  # We prefer static libraries, so we reverse it here.
  LIST(REVERSE CMAKE_FIND_LIBRARY_SUFFIXES)

  FIND_LIBRARY(LDAP_CUSTOM_LIBRARY
    NAMES ldap_r ldap
    PATHS ${WITH_LDAP_PATH}/lib
    NO_DEFAULT_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    )
  FIND_LIBRARY(LBER_CUSTOM_LIBRARY
    NAMES lber
    PATHS ${WITH_LDAP_PATH}/lib
    NO_DEFAULT_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    )

  LIST(REVERSE CMAKE_FIND_LIBRARY_SUFFIXES)

  IF(LDAP_INCLUDE_DIR)
    INCLUDE_DIRECTORIES(BEFORE SYSTEM ${LDAP_INCLUDE_DIR})

    CMAKE_PUSH_CHECK_STATE()
    SET(CMAKE_REQUIRED_INCLUDES ${LDAP_INCLUDE_DIR})
    CHECK_INCLUDE_FILE(lber.h HAVE_LBER_H)
    CHECK_INCLUDE_FILE(ldap.h HAVE_LDAP_H)
    CMAKE_POP_CHECK_STATE()
  ENDIF()

  IF(LDAP_CUSTOM_LIBRARY AND LBER_CUSTOM_LIBRARY)
    SET(LDAP_LIBRARY ${LDAP_CUSTOM_LIBRARY})
    SET(LBER_LIBRARY ${LBER_CUSTOM_LIBRARY})
    GET_FILENAME_COMPONENT(LDAP_LIBRARY_EXT ${LDAP_LIBRARY} EXT)
    IF(LDAP_LIBRARY_EXT STREQUAL ".a")
      SET(STATIC_LDAP_LIBRARY 1)
    ENDIF()
    MESSAGE(STATUS "LDAP_LIBRARY ${LDAP_LIBRARY}")
    MESSAGE(STATUS "LBER_LIBRARY ${LBER_LIBRARY}")
  ENDIF()
ENDMACRO()

MACRO(MYSQL_CHECK_LDAP)
  IF(NOT WITH_LDAP)
    SET(WITH_LDAP "system" CACHE STRING ${WITH_LDAP_DOC_STRING} FORCE)
  ENDIF()

  # See if WITH_LDAP is of the form </path/to/custom/installation>
  FILE(GLOB WITH_LDAP_HEADER ${WITH_LDAP}/include/ldap.h)
  IF (WITH_LDAP_HEADER)
    FILE(TO_CMAKE_PATH "${WITH_LDAP}" WITH_LDAP)
    SET(WITH_LDAP_PATH ${WITH_LDAP})
  ENDIF()

  IF(WITH_LDAP STREQUAL "system")
    FIND_SYSTEM_LDAP()
  ELSE()
    FIND_CUSTOM_LDAP()
  ENDIF()

  IF(WIN32 AND WITH_LDAP STREQUAL "system")
    SET(LDAP_FOUND 1)
  ELSEIF(HAVE_LBER_H AND HAVE_LDAP_H AND LDAP_LIBRARY AND LBER_LIBRARY)
    SET(LDAP_FOUND 1)
  ELSE()
    SET(LDAP_FOUND 0)
  ENDIF()

ENDMACRO()
