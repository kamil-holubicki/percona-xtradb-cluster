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

#ifndef AUDIT_LOG_FILTER_LOG_WRITER_FILE_H_INCLUDED
#define AUDIT_LOG_FILTER_LOG_WRITER_FILE_H_INCLUDED

#include "base.h"
#include "file_handle.h"

#include <string>

namespace audit_log_filter {

namespace log_writer_strategy {
class FileWriterStrategyBase;
}

namespace log_writer {

template <>
class LogWriter<AuditLogHandlerType::File> : public LogWriterBase {
 public:
  LogWriter<AuditLogHandlerType::File>() = delete;
  explicit LogWriter<AuditLogHandlerType::File>(
      std::unique_ptr<log_record_formatter::LogRecordFormatterBase> formatter);
  ~LogWriter<AuditLogHandlerType::File>() override;

  /**
   * @brief Open log writer.
   *
   * @return true in case of success, false otherwise
   */
  bool open() noexcept override;

  /**
   * @brief Close log writer.
   *
   * @return true in case of success, false otherwise
   */
  bool close() noexcept override;

  /**
   * @brief Write audit record to log.
   *
   * @param record String representation of audit record
   * @param print_separator Add lor record separator before a record
   *                        if set to true
   */
  void write(const std::string &record, bool print_separator) noexcept override;

  /**
   * @brief Get current log file size in bytes.
   *
   * @return Current log file size in bytes
   */
  [[nodiscard]] uint64_t get_log_size() const noexcept override;

  /**
   * @brief Prune outdated log files.
   */
  void prune() noexcept override;

 private:
  /**
   * @brief Implement actual file opening logic.
   *
   * @return true in case file opened successfully, false otherwise
   */
  bool do_open_file() noexcept;

  /**
   * @brief Implement actual file closing logic.
   *
   * @return true in case file closed successfully, false otherwise
   */
  bool do_close_file() noexcept;

  /**
   * @brief Rotate log file.
   */
  void rotate() noexcept override;

 private:
  bool m_is_rotating;
  bool m_is_log_empty;
  bool m_is_opened;
  FileHandle m_file_handle;
  std::unique_ptr<log_writer_strategy::FileWriterStrategyBase> m_strategy;
};

using LogWriterFile = LogWriter<AuditLogHandlerType::File>;

}  // namespace log_writer
}  // namespace audit_log_filter

#endif  // AUDIT_LOG_FILTER_LOG_WRITER_FILE_H_INCLUDED
