/* Copyright (c) 2022 Percona LLC and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

#ifndef AUDIT_LOG_FILTER_LOG_RECORD_FORMATTER_BASE_H_INCLUDED
#define AUDIT_LOG_FILTER_LOG_RECORD_FORMATTER_BASE_H_INCLUDED

#include "plugin/audit_log_filter/audit_event_class_internal.h"

#include "mysql/plugin_audit.h"

#include <atomic>
#include <chrono>
#include <map>
#include <sstream>
#include <string>
#include <string_view>

namespace audit_log_filter {

struct AuditRecordGeneral;
struct AuditRecordConnection;
struct AuditRecordTableAccess;
struct AuditRecordGlobalVariable;
struct AuditRecordServerStartup;
struct AuditRecordServerShutdown;
struct AuditRecordCommand;
struct AuditRecordQuery;
struct AuditRecordStoredProgram;
struct AuditRecordAuthentication;
struct AuditRecordMessage;
struct AuditRecordStartAudit;
struct AuditRecordStopAudit;
struct AuditRecordUnknown;

namespace log_record_formatter {

using EscapeRulesContainer = std::map<char, std::string>;
using AuditRecordString = std::string;

enum class AuditLogFormatType {
  New,
  Old,
  Json,
  Csv,
  FormatsCount  // This item must be last in the list
};

class LogRecordFormatterBase {
 public:
  virtual ~LogRecordFormatterBase() = default;

  /**
   * @brief Apply formatting to AuditRecordGeneral audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordGeneral &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordConnection audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordConnection &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordTableAccess audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordTableAccess &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordGlobalVariable audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordGlobalVariable &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordServerStartup audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordServerStartup &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordServerShutdown audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordServerShutdown &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordCommand audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordCommand &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordQuery audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordQuery &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordStoredProgram audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordStoredProgram &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordAuthentication audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordAuthentication &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordMessage audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordMessage &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordStartAudit audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordStartAudit &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordStopAudit audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] virtual AuditRecordString apply(
      const AuditRecordStopAudit &audit_record) const noexcept = 0;

  /**
   * @brief Apply formatting to AuditRecordUnknown audit record.
   *
   * @param [in] audit_record Audit record
   * @return String representing formatted audit record
   */
  [[nodiscard]] AuditRecordString apply(
      const AuditRecordUnknown &audit_record) const noexcept;

  /**
   * @brief Get log file header string.
   *
   * @return Log file header string
   */
  [[nodiscard]] virtual std::string get_file_header() const noexcept = 0;

  /**
   * @brief Get log file footer string.
   *
   * @return Log file footer string
   */
  [[nodiscard]] virtual std::string get_file_footer() const noexcept = 0;

  /**
   * @brief Get separator added between event records in a log file.
   *
   * @return Event records separator string
   */
  [[nodiscard]] virtual std::string get_record_separator() const noexcept = 0;

  /**
   * @brief Insert audit event class and subclass names into record printed to
   *        log. Needed for testing.
   *
   * @param event_class_name Event class name
   * @param event_subclass_name Event subclass name
   * @param record_str String representation of audit record
   */
  virtual void apply_debug_info(std::string_view event_class_name,
                                std::string_view event_subclass_name,
                                std::string &record_str) noexcept = 0;

 protected:
  /**
   * @brief Get timestamp string representation.
   *
   * @param time_point Time point
   * @return Timestamp string
   */
  [[nodiscard]] virtual std::string make_timestamp(
      std::chrono::system_clock::time_point time_point) const noexcept = 0;

  /**
   * @brief Get record ID string representation.
   *
   * @param time_point Time point
   * @return Record ID string
   */
  [[nodiscard]] std::string make_record_id(
      std::chrono::system_clock::time_point time_point) const noexcept;

  /**
   * @brief Get numeric record ID.
   *
   * @return Record ID
   */
  [[nodiscard]] uint64_t make_record_id() const noexcept;

  /**
   * @brief Apply escaping rules to provided string.
   *
   * @param in String to be escaped
   * @return Escaped string
   */
  [[nodiscard]] std::string make_escaped_string(
      const std::string &in) const noexcept;

  /**
   * @brief Apply escaping rules to provided string.
   *
   * @param in String to be escaped
   * @return Escaped string
   */
  [[nodiscard]] std::string make_escaped_string(
      const MYSQL_LEX_CSTRING *in) const noexcept;

  /**
   * @brief Get string representation of audit event class name.
   *
   * @param event_class Audit event class
   * @return String representation of audit event class name
   */
  [[nodiscard]] static std::string_view event_class_to_string(
      mysql_event_class_t event_class) noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      audit_filter_event_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_general_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_connection_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_table_access_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_global_variable_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_command_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_query_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_authentication_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_server_startup_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_server_shutdown_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_stored_program_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of audit event subclass name.
   *
   * @param event_subclass Audit event subclass
   * @return String representation of audit event subclass name
   */
  [[nodiscard]] virtual std::string_view event_subclass_to_string(
      mysql_event_message_subclass_t event_subclass) const noexcept;

  /**
   * @brief Get string representation of connection type name.
   *
   * @param connection_type Connection type
   * @return String representation of connection type name
   */
  [[nodiscard]] virtual std::string_view connection_type_name_to_string(
      int connection_type) const noexcept;

  /**
   * @brief Get string representation of shutdown reason.
   *
   * @param reason Shutdown reason
   * @return String representation of shutdown reason
   */
  [[nodiscard]] virtual std::string_view shutdown_reason_to_string(
      mysql_server_shutdown_reason_t reason) const noexcept;

  /**
   * @brief Get string representation of SQL command ID.
   *
   * @param sql_command_id SQL command ID
   * @return String representation of SQL command ID
   */
  [[nodiscard]] static std::string_view sql_command_id_to_string(
      enum_sql_command_t sql_command_id) noexcept;

  /**
   * @brief Get string representation of command ID.
   *
   * @param command_id Command ID
   * @return String representation of command ID
   */
  [[nodiscard]] static std::string_view command_id_to_string(
      enum_server_command_t command_id) noexcept;

 private:
  /**
   * @brief Get escape rules.
   *
   * @return Escape rules
   */
  virtual const EscapeRulesContainer &get_escape_rules() const noexcept = 0;
};

class LogRecordFormatterBaseXml : public LogRecordFormatterBase {
 public:
  /**
   * @brief Get log file header string.
   *
   * @return Log file header string
   */
  [[nodiscard]] std::string get_file_header() const noexcept override;

  /**
   * @brief Get log file footer string.
   *
   * @return Log file footer string
   */
  [[nodiscard]] std::string get_file_footer() const noexcept override;

  /**
   * @brief Get separator added between event records in a log file.
   *
   * @return Event resords separator string
   */
  [[nodiscard]] std::string get_record_separator() const noexcept override;

 protected:
  /**
   * @brief Get timestamp string representation.
   *
   * @param time_point Time point
   * @return Timestamp string
   */
  [[nodiscard]] std::string make_timestamp(
      std::chrono::system_clock::time_point time_point) const noexcept override;

 private:
  /**
   * @brief Get escape rules.
   *
   * @return Escape rules
   */
  [[nodiscard]] const EscapeRulesContainer &get_escape_rules()
      const noexcept override;
};

template <AuditLogFormatType FormatType>
class LogRecordFormatter;

}  // namespace log_record_formatter
}  // namespace audit_log_filter

#endif  // AUDIT_LOG_FILTER_LOG_RECORD_FORMATTER_BASE_H_INCLUDED
