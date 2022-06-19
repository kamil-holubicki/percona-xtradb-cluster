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

#ifndef AUDIT_LOG_FILTER_SYS_VARS_H_INCLUDED
#define AUDIT_LOG_FILTER_SYS_VARS_H_INCLUDED

#include "plugin/audit_log_filter/audit_base_component.h"
#include "plugin/audit_log_filter/log_record_formatter/base.h"
#include "plugin/audit_log_filter/log_writer/base.h"
#include "plugin/audit_log_filter/log_writer_strategy/base.h"

#include <memory>
#include <string>

namespace audit_log_filter {

class SysVarServices;
class SysVars;

using log_record_formatter::AuditLogFormatType;
using log_writer::AuditLogHandlerType;
using log_writer_strategy::AuditLogStrategyType;

template <typename T>
class VarWrapper {
 public:
  explicit VarWrapper(T def_value) : m_value{def_value}, m_container{nullptr} {}

  VarWrapper &operator=(T val) noexcept {
    m_value = val;
    return *this;
  }

  explicit operator const T &() const noexcept { return m_value; }
  explicit operator T &() noexcept { return m_value; }

  [[nodiscard]] SysVars *get_container() const noexcept { return m_container; }
  void set_container(SysVars *container) noexcept { m_container = container; }

 private:
  T m_value;
  SysVars *m_container;
};

class SysVars : public AuditBaseComponent {
 public:
  SysVars() = delete;
  explicit SysVars(std::unique_ptr<SysVarServices> sys_var_services);
  ~SysVars();

  /**
   * @brief Init system variables.
   *
   * @return true in case system variables initialised successfully,
   *         false otherwise
   */
  bool init() noexcept;

  /**
   * @brief Get audit log filter file base name.
   *
   * @return Audit log filter file base name
   */
  [[nodiscard]] const char *get_file_name() noexcept { return m_file_name; }

  /**
   * @brief Get audit log filter handler type.
   *
   * @return Audit log filter handler type, may be one of possible values
   *         of AuditLogHandlerType
   */
  [[nodiscard]] AuditLogHandlerType get_handler_type() noexcept {
    return m_handler_type;
  }

  /**
   * @brief Get audit log filter format type.
   *
   * @return Audit log filter format type, may be one of possible values
   *         of AuditLogFormatType
   */
  [[nodiscard]] AuditLogFormatType get_format_type() noexcept {
    return m_format_type;
  }

  /**
   * @brief Get audit log filter file logging strategy.
   *
   * @return Audit log filter file logging strategy, may be one of possible
   *         values of AuditLogStrategyType
   */
  [[nodiscard]] AuditLogStrategyType get_file_strategy() noexcept {
    return m_file_stategy;
  }

  /**
   * @brief Get size of memory buffer used for logging in bytes.
   *
   * @return Size of memory buffer used for logging in bytes
   */
  [[nodiscard]] ulonglong get_buffer_size() const noexcept {
    return m_buffer_size;
  }

  /**
   * @brief Get the maximum size of the audit filter log file in bytes.
   *
   * @return Maximum size of the audit filter log file in bytes
   */
  [[nodiscard]] ulonglong get_rotate_on_size() const noexcept {
    return m_rotate_on_size;
  }

  /**
   * @brief Get the number of log files which should be kept when rotations
   *        are done.
   *
   * @return Number of rotated log files kept on disk
   */
  [[nodiscard]] ulonglong get_rotations() const noexcept { return m_rotations; }

  /**
   * @brief Get the ident value for syslog.
   *
   * @return Ident value for syslog
   */
  [[nodiscard]] const char *get_syslog_ident() noexcept {
    return m_syslog_ident;
  }

  /**
   * @brief Get the facility value for syslog.
   *
   * @return Facility value for syslog
   */
  [[nodiscard]] int get_syslog_facility() const noexcept;

  /**
   * @brief Get the priority value for syslog.
   *
   * @return Priority value for syslog
   */
  [[nodiscard]] int get_syslog_priority() const noexcept;

 private:
  std::unique_ptr<SysVarServices> m_sys_var_services;

  char *m_file_name = nullptr;
  AuditLogHandlerType m_handler_type = AuditLogHandlerType::File;
  AuditLogFormatType m_format_type = AuditLogFormatType::New;
  AuditLogStrategyType m_file_stategy = AuditLogStrategyType::Asynchronous;
  ulonglong m_buffer_size = 1048576UL;
  ulonglong m_rotate_on_size = 0UL;
  ulonglong m_rotations = 0UL;
  char *m_syslog_ident = nullptr;
  ulong m_syslog_facility = 0UL;
  ulong m_syslog_priority = 0UL;
  VarWrapper<bool> m_log_flush_requested;
};

}  // namespace audit_log_filter

#endif  // AUDIT_LOG_FILTER_SYS_VARS_H_INCLUDED
