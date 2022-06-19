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

#ifndef AUDIT_LOG_FILTER_LOG_WRITER_SYSLOG_H_INCLUDED
#define AUDIT_LOG_FILTER_LOG_WRITER_SYSLOG_H_INCLUDED

#include "base.h"

namespace audit_log_filter::log_writer {

template <>
class LogWriter<AuditLogHandlerType::Syslog> : public LogWriterBase {
 public:
  LogWriter() = delete;
  LogWriter(
      std::unique_ptr<log_record_formatter::LogRecordFormatterBase> formatter,
      const LogWriterConfig &conf);

  /**
   * @brief Open log writer.
   *
   * @return true in case of success, false otherwise
   */
  bool open() noexcept override { return true; }

  /**
   * @brief Close log writer.
   *
   * @return true in case of success, false otherwise
   */
  bool close() noexcept override { return true; }

  /**
   * @brief Write audit record to log.
   *
   * @param record String representation of audit record
   */
  void write(const std::string &record [[maybe_unused]]) noexcept override {}

  /**
   * @brief Close and reopen current log file. Used for manual log rotation.
   */
  void flush() noexcept override {}

  /**
   * @brief Get current log file size in bytes.
   *
   * @return Current log file size in bytes
   */
  [[nodiscard]] uint64_t get_log_size() const noexcept override { return 0; }

 private:
  std::string m_syslog_ident;
  int m_syslog_facility;
  int m_syslog_priority;
};

using LogWriterSyslog = LogWriter<AuditLogHandlerType::Syslog>;

}  // namespace audit_log_filter::log_writer

#endif  // AUDIT_LOG_FILTER_LOG_WRITER_SYSLOG_H_INCLUDED
