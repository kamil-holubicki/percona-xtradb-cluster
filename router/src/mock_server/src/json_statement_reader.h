/*
  Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.

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
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MYSQLD_MOCK_JSON_STATEMENT_READER_INCLUDED
#define MYSQLD_MOCK_JSON_STATEMENT_READER_INCLUDED

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "statement_reader.h"

namespace server_mock {

/** @class QueriesJsonReader
 *
 * @brief  Responsible for reading the json file with
 *         the expected statements and simplifying the data
 *         structures used by RapidJSON into vectors.
 **/
class QueriesJsonReader : public StatementReaderBase {
 public:
  enum class HandshakeState { INIT, GREETED, AUTH_SWITCHED, DONE };

  /**
   * Constructor.
   *
   * @param filename Path to the json file with definitins
   *         of the expected SQL statements and responses
   **/
  QueriesJsonReader(const std::string &filename);

  /**
   * handle handshake's init state.
   *
   * @param payload payload of the current client packet
   * @param next_state next state of the handshake handler
   *
   * @return response to send to client
   */
  HandshakeResponse handle_handshake_init(const std::vector<uint8_t> &payload,
                                          HandshakeState &next_state);

  /**
   * handle handshake's greeted state.
   *
   * @param payload payload of the current client packet
   * @param next_state next state of the handshake handler
   *
   * @return response to send to client
   */
  HandshakeResponse handle_handshake_greeted(
      const std::vector<uint8_t> &payload, HandshakeState &next_state);

  /**
   * handle handshake's auth_switched state.
   *
   * @param payload payload of the current client packet
   * @param next_state next state of the handshake handler
   *
   * @return response to send to client
   */
  HandshakeResponse handle_handshake_auth_switched(
      const std::vector<uint8_t> &payload, HandshakeState &next_state);

  /**
   * handle the handshake payload received from the client.
   *
   * @param payload payload of the client's current handshake packet
   * @returns response to send to client
   */
  HandshakeResponse handle_handshake(
      const std::vector<uint8_t> &payload) override;

  /**
   * Returns the data about the next statement from the
   * json file.
   *
   * @param statement statement-text of the current clients
   *COM_QUERY/StmtExecute
   * @returns response to send to client
   **/
  StatementResponse handle_statement(const std::string &statement) override;

  /** @brief Returns the default execution time in microseconds. If
   *         no default execution time is provided in json file, then
   *         0 microseconds is returned.
   **/
  std::chrono::microseconds get_default_exec_time() override;

  ~QueriesJsonReader() override;

 private:
  // This is to avoid including RapidJSON headers here, which would cause
  // them included also in other files (they give tons of warnings, which
  // better suppres only for single implementation file).
  struct Pimpl;
  std::unique_ptr<Pimpl> pimpl_;

  HandshakeState handshake_state_{HandshakeState::INIT};
};

}  // namespace server_mock

#endif  // MYSQLD_MOCK_JSON_STATEMENT_READER_INCLUDED
