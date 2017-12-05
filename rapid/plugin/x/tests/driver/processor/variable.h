/*
 * Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef RAPID_PLUGIN_X_TESTS_DRIVER_PROCESSOR_VARIABLE_H_
#define RAPID_PLUGIN_X_TESTS_DRIVER_PROCESSOR_VARIABLE_H_

#include <functional>
#include <string>


class Variable_interface {
 public:
  virtual ~Variable_interface() = default;

  virtual bool set_value(const std::string &value) = 0;
  virtual std::string get_value() const = 0;
};


class Variable_string: public Variable_interface {
 public:
  Variable_string() = default;

  explicit Variable_string(const std::string &value)
  : m_value(value) {
  }

  explicit Variable_string(std::string &&value)
  : m_value(value) {
  }

  bool set_value(const std::string &value) override {
    m_value = value;

    return true;
  }

  std::string get_value() const override {
    return m_value;
  }

 private:
  std::string m_value;
};


class Variable_string_readonly: public Variable_string {
 public:
  explicit Variable_string_readonly(const std::string &value)
  : Variable_string(value) {}

  template <typename Value_type>
  explicit Variable_string_readonly(const Value_type &value)
  : Variable_string(std::to_string(value)) {}

  bool set_value(const std::string &value) override {
    return false;
  }
};


class Variable_dynamic_string: public Variable_interface {
 public:
  using String_ref = std::reference_wrapper<std::string>;

  explicit Variable_dynamic_string(const String_ref &value_ref)
  : m_value(value_ref) {
  }

  bool set_value(const std::string &value) override {
    m_value.get() = value;

    return true;
  }

  std::string get_value() const override {
    return m_value.get();
  }

 private:
  String_ref m_value;
};


class Variable_dynamic_int: public Variable_interface {
 public:
  using Int_ref = std::reference_wrapper<int>;

  explicit Variable_dynamic_int(const Int_ref &value_ref)
  : m_value(value_ref) {
  }

  bool set_value(const std::string &value) override {
    try {
      m_value.get() = std::stoi(value);
    }
    catch (const std::exception &) {
      return false;
    }

    return true;
  }

  std::string get_value() const override {
    return std::to_string(m_value.get());
  }

 private:
  Int_ref m_value;
};


#endif  // RAPID_PLUGIN_X_TESTS_DRIVER_PROCESSOR_VARIABLE_H_
