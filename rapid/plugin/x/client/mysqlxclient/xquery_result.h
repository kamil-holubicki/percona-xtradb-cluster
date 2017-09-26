/*
 * Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

// MySQL DB access module, for use by plugins and others
// For the module that implements interactive DB functionality see mod_db

#ifndef X_CLIENT_MYSQLXCLIENT_XQUERY_RESULT_H_
#define X_CLIENT_MYSQLXCLIENT_XQUERY_RESULT_H_

#include <memory>
#include <string>
#include <vector>

#include "mysqlxclient/xerror.h"
#include "mysqlxclient/xrow.h"


namespace xcl {

/**
  Interface responsible for fetching 'resultsets'.

  The interface defines methods that correspond to X Protocol message flow
  which transmits multiple 'resultsets':

      loop has more resultsets
        group resultset
          loop has more columns
            server --> client: ColumnMetaData
          end
          loop has more rows
            server --> client: Row
          end
        end
        alt has more resultsets
          server --> client: FetchDoneMoreResultsets
        end
      end
      loop has more OUT-paramsets
        server --> client: FetchDoneMoreOutParams
        group resultset
          loop has more columns
            server --> client: ColumnMetaData
          end
          loop has more rows
            server --> client: Row
          end
        end
      end
      server --> client: FetchDone

  While designing it there was an assumption that the implementation may not
  fetch/buffers row data. Because of it some return containers may "grow" with
  the realization of the X Protocol flow.
*/
class XQuery_result {
 public:
  using Warning  = ::Mysqlx::Notice::Warning;
  using Row      = ::Mysqlx::Resultset::Row;
  using Metadata = std::vector<Column_metadata>;
  using Warnings = std::vector<Warning>;

 public:
  virtual ~XQuery_result() = default;

  /**
    Get last insert identifier.

    If last insert identifier was generated by the query then
    it is going to be send in few last messages of the statement
    execution flow thus the value can be checked after last 'resultset'
    was received (@ref next_resultset returned false). Before this point
    the method is going to return 'false'.

    @param[out] out_last_id   returns last insert identifier

    @return Information if the last insert identifier is defined for
            last 'resultset'
      @retval == true    last insert id is defined
      @retval == false   last insert id is not defined
  */
  virtual bool try_get_last_insert_id(uint64_t *out_last_id) const = 0;

  /**
    Get number of affected rows.

    If affected row count was generated by the query then
    it is going to be send in few last messages of the statement
    execution flow thus the value can be checked after last 'resultset'
    was received (@ref next_resultset returned false). Before this point
    the method is going to return 'false'.

    @param[out] out_affected_number   returns last insert identifier

    @return Information if the affected number of rows is defined for
            last 'resultset'
      @retval == true    number of affected rows is defined
      @retval == false   number of affected rows is not defined
  */
  virtual bool try_get_affected_rows(uint64_t *out_affected_number) const = 0;

  /**
    Get message returned for the server.

    If the message was generated by the query then it is going to be send
    in few last messages of the statement execution flow thus the value
    can be checked after last 'resultset' was received (@ref next_resultset
    returned false). Before this point the method is going to return 'false'.

    @param[out] out_message   returns message

    @return Information if the message is defined for last 'resultset'
      @retval == true    number of affected rows is defined
      @retval == false   number of affected rows is not defined
  */
  virtual bool try_get_info_message(std::string *out_message) const = 0;

  /**
    Get column information for current 'resultset'.

    Returns column information in case when there was no error
    earlier and fetch of metadata was successful.
    The method doesn't need to be called before call to "get_next_row*",
    it is done implicitly in that case and the metadata-s are cached.

    @param[out] out_error   return error in case the fetch operation fails
                            or last error. To omit the value the method
                            accepts "nullptr" value.

    @return Container holding Column_metadata for all columns
  */
  virtual const Metadata &get_metadata(XError *out_error = nullptr) = 0;

  /**
    Get warning generated for current 'resultset'.

    Warnings are generated by X Plugin or SQL executed on the server
    and are forwarded to the client through X Protocol notices.
    Number of warnings is going to increase with flow realization and
    may be generated while serializing 'row', 'field'.

    @return Container holding warnings generated until now
  */
  virtual const Warnings &get_warnings() = 0;

  /**
    Get next row of the current 'resultset'.

    @param[out] out_row    returns a row pointer from the internal storage,
                           if the value is 'nullptr' then row is skipped
    @param[out] out_error  returns an error when fetching of row failed
                           or last error

    @return Result of row fetching
      @retval == true   out_row contains a valid row
      @retval == false  there are no rows to fetch in current 'resultset' or
                        an error occurred
  */
  virtual bool get_next_row(const XRow **out_row, XError *out_error) = 0;

  /**
    Get next row of the current 'resultset'.

    @param[out] out_error  returns an error when fetching of row failed
                           or last error

    @return Result of row fetching
      @retval != nullptr  pointer to fetched row owned by this interface
      @retval == nullptr  there are no rows to fetch in current 'resultset' or
                          an error occurred
  */
  virtual const XRow *get_next_row(XError *out_error = nullptr) = 0;

  /**
    Get next row of the current 'resultset' as X Protocol message.

    @param[out] out_error  returns an error when fetching of row failed
                           or last error

    @return Result of row fetching
      @retval != nullptr  pointer to fetched X Protocol message
      @retval == nullptr  there are no rows to fetch in current 'resultset' or
                          an error occurred
  */
  virtual std::unique_ptr<Row> get_next_row_raw(
      XError *out_error = nullptr) = 0;

  /**
    Move to next 'resultset'.

    Method checks if the executed query generated multiple/next "resultset/s".
    It returns "true" if the next "resultset" is present and can be read
    by calling once again by methods like @ref get_metadata,
    @ref get_warnings, @ref get_next_row, @ref get_next_row_raw.
    In case of "false", user should check out_error to see if the cause of
    stopping was an error or an end of resultsets.
    Using this method user can skip in middle of current resultset and
    go to next one or even skip whole resultset.

    @param[out] out_error   returns an error if it occurred while
                            skipping/moving message flow to the next
                            'resultset' or last error

    @return Information if user is able to read next 'resultset'
      @retval == true       next 'resultset' is available
      @retval == false      all 'resultset' received or an error occur
  */
  virtual bool next_resultset(XError *out_error) = 0;


  /**
    Check if there is a 'resultset'.

    Method tries to fetch 'resultset' metadata to check if there
    is a resultset.

    @param[out] out_error   returns an error if it occurred while
                            checking the resultset
  */
  virtual bool has_resultset(XError *out_error = nullptr) = 0;
};

}  // namespace xcl

#endif  // X_CLIENT_MYSQLXCLIENT_XQUERY_RESULT_H_
