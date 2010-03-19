# Copyright (C) 2010 Sun Microsystems, Inc
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

# The purpose of this file is to set the default installation layout.
# Currently, there are 2 different installation layouts ,
# one is used in tar.gz packages (Windows zip is about the same), another one 
# in RPMs.

# There are currently 2 layouts defines, named STANDALONE (tar.gz layout)
# and UNIX (rpm layout). To force a directory layout when invoking cmake use
# -DINSTALL_LAYOUT=[STANDALONE|UNIX].
# This wil use a predefined layout. There is a possibility to further fine-tune 
# installation directories. Several variables are can be overwritten
#
# - INSTALL_BINDIR     (directory with client executables and Unix shell scripts)
# - INSTALL_SBINDIR    (directory with mysqld)
# - INSTALL_LIBDIR     (directory with client end embedded libraries)
# - INSTALL_PLUGINDIR  (directory for plugins)
# - INSTALL_INCLUDEDIR (directory for MySQL headers)
# - INSTALL_DOCDIR     (documentation)
# - INSTALL_MANDIR     (man pages)
# - INSTALL_SCRIPTDIR  (several scripts, rarely used)
# - INSTALL_MYSQLSHAREDIR (MySQL character sets and localized error messages)
# - INSTALL_SHAREDIR  (location of aclocal/mysql.m4)
# - INSTALL_SQLBENCHDIR (sql-bench)
# - INSTALL_MYSQLTESTDIR (mysql-test)
# - INSTALL_DOCREADMEDIR (readme and similar)
# - INSTALL_SUPPORTFILESDIR (used only in standalone installer)

# Default installation layout on Unix is UNIX (kent wants it so)
IF(NOT INSTALL_LAYOUT)
  IF(WIN32)
    SET(DEFAULT_INSTALL_LAYOUT "STANDALONE")
  ELSE()
    SET(DEFAULT_INSTALL_LAYOUT "UNIX")
  ENDIF()
ENDIF()

SET(INSTALL_LAYOUT "${DEFAULT_INSTALL_LAYOUT}" 
CACHE STRING  "Installation directory layout. Options are: STANDALONE (as in zip or tar.gz installer) or UNIX")

IF(NOT INSTALL_LAYOUT MATCHES "STANDALONE")
  IF(NOT INSTALL_LAYOUT MATCHES "UNIX")
    # XXX not keen on this, how many more will we need? prefer to just bail...
    IF(NOT INSTALL_LAYOUT MATCHES "SVR4")
      SET(INSTALL_LAYOUT "${DEFAULT_INSTALL_LAYOUT}")
  ENDIF()
ENDIF()

IF(UNIX)
  IF(INSTALL_LAYOUT MATCHES "UNIX")  
    SET(default_prefix "/usr")
  ELSEIF(INSTALL_LAYOUT MATCHES "SVR4")
    SET(default_prefix "/opt/mysql/mysql")
  ELSE()
    SET(default_prefix "/usr/local/mysql")
  ENDIF()
  IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX ${default_prefix} 
      CACHE PATH "install prefix" FORCE)
  ENDIF()
  SET(SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc" 
    CACHE PATH "config directory (for my.cnf)")
  MARK_AS_ADVANCED(SYSCONFDIR)
ENDIF()

 
 
 # STANDALONE layout
 SET(INSTALL_BINDIR_STANDALONE "bin")
 SET(INSTALL_SBINDIR_STANDALONE "bin")
 SET(INSTALL_LIBDIR_STANDALONE "lib")
 SET(INSTALL_INCLUDEDIR_STANDALONE "include")
 SET(INSTALL_PLUGINDIR_STANDALONE "lib/plugin")
 SET(INSTALL_DOCDIR_STANDALONE "docs")
 SET(INSTALL_MANDIR_STANDALONE "man")
 SET(INSTALL_MYSQLSHAREDIR_STANDALONE "share")
 SET(INSTALL_SHAREDIR_STANDALONE "share")
 SET(INSTALL_SCRIPTDIR_STANDALONE "scripts")
 SET(INSTALL_MYSQLTESTDIR_STANDALONE "mysql-test")
 SET(INSTALL_SQLBENCHROOTDIR_STANDALONE ".")
 SET(INSTALL_DOCREADMEDIR_STANDALONE ".")
 SET(INSTALL_SUPPORTFILESDIR_STANDALONE "support-files")
 SET(INSTALL_MYSQLDATADIR_STANDALONE "data")
 
 # SVR4 layout
 SET(INSTALL_BINDIR_SVR4 "bin")
 SET(INSTALL_SBINDIR_SVR4 "bin")
 SET(INSTALL_LIBDIR_SVR4 "lib")
 SET(INSTALL_INCLUDEDIR_SVR4 "include")
 SET(INSTALL_PLUGINDIR_SVR4 "lib/plugin")
 SET(INSTALL_DOCDIR_SVR4 "docs")
 SET(INSTALL_MANDIR_SVR4 "man")
 SET(INSTALL_MYSQLSHAREDIR_SVR4 "share")
 SET(INSTALL_SHAREDIR_SVR4 "share")
 SET(INSTALL_SCRIPTDIR_SVR4 "scripts")
 SET(INSTALL_MYSQLTESTDIR_SVR4 "mysql-test")
 SET(INSTALL_SQLBENCHROOTDIR_SVR4 ".")
 SET(INSTALL_DOCREADMEDIR_SVR4 ".")
 SET(INSTALL_SUPPORTFILESDIR_SVR4 "support-files")
 SET(MYSQL_DATADIR "/var/lib/mysql")
 
 # UNIX layout
 SET(INSTALL_BINDIR_UNIX "bin")
 SET(INSTALL_SBINDIR_UNIX "sbin")
 SET(INSTALL_LIBDIR_UNIX "lib/mysql")
 SET(INSTALL_PLUGINDIR_UNIX "lib/mysql/plugin")
 SET(INSTALL_DOCDIR_UNIX "share/mysql/doc/MySQL-server-${MYSQL_NO_DASH_VERSION}")
 SET(INSTALL_MANDIR_UNIX "share/mysql/man")
 SET(INSTALL_INCLUDEDIR_UNIX "include/mysql")
 SET(INSTALL_MYSQLSHAREDIR_UNIX "share/mysql")
 SET(INSTALL_SHAREDIR_UNIX "share")
 SET(INSTALL_SCRIPTDIR_UNIX "bin")
 SET(INSTALL_MYSQLTESTDIR_UNIX "mysql-test")
 SET(INSTALL_SQLBENCHROOTDIR_UNIX "")
 SET(INSTALL_DOCREADMEDIR_UNIX "share/mysql/doc/MySQL-server-${MYSQL_NO_DASH_VERSION}")
 SET(INSTALL_SUPPORTFILESDIR_UNIX "")
 SET(INSTALL_MYSQLDATADIR_UNIX "var")


# Clear cached variables if install layout was changed
IF(OLD_INSTALL_LAYOUT)
  IF(NOT OLD_INSTALL_LAYOUT STREQUAL INSTALL_LAYOUR)
    SET(FORCE FORCE)
  ENDIF()
ENDIF()
SET(OLD_INSTALL_LAYOUT ${INSTALL_LAYOUT} CACHE INTERNAL "")

# Set INSTALL_FOODIR variables for chosen  layout
# (for example,  INSTALL_BINDIR will be defined  as 
# ${INSTALL_BINDIR_STANDALONE} by  default if STANDALONE layout is chosen)
FOREACH(var BIN SBIN LIB MYSQLSHARE SHARE PLUGIN INCLUDE SCRIPT DOC MAN 
  MYSQLTEST SQLBENCHROOT DOCREADME SUPPORTFILES MYSQLDATA)
  SET(INSTALL_${var}DIR  ${INSTALL_${var}DIR_${INSTALL_LAYOUT}} 
  CACHE STRING "${var} installation directory" ${FORCE})
  MARK_AS_ADVANCED(INSTALL_${var}DIR)
ENDFOREACH()
