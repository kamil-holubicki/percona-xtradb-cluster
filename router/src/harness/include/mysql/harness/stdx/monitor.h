/*
  Copyright (c) 2020, Oracle and/or its affiliates.

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

#ifndef MYSQL_HARNESS_STDX_MONITOR_H_INCLUDED
#define MYSQL_HARNESS_STDX_MONITOR_H_INCLUDED

#include <condition_variable>
#include <mutex>
#include <utility>  // move

/**
 * Monitor pattern.
 *
 * implemented based on Herb Sutters example.
 */
template <class T>
class Monitor {
 public:
  Monitor(T t) : t_{std::move(t)} {}

  template <class F>
  auto operator()(F f) const {
    std::lock_guard<std::mutex> lk{mtx_};

    return f(t_);
  }

 protected:
  mutable T t_;

  mutable std::mutex mtx_;
};

/**
 * Wrapper to make Base notifyable.
 *
 * adds a method 'notify_one()' to the wrapped class.
 *
 * used by WaitableMonitor.
 */
template <class Base>
class Notifyable : public Base {
 public:
  template <class... Args>
  Notifyable(std::condition_variable &cv, Args &&... args)
      : Base{std::forward<Args>(args)...}, cv_{cv} {}

  void notify_one() noexcept { cv_.notify_one(); }

 private:
  std::condition_variable &cv_;
};

/**
 * Monitor can be waited for.
 *
 * wraps T by with Notifyable to add '.notify_one' to T.
 */
template <class T>
class WaitableMonitor : public Monitor<Notifyable<T>> {
 public:
  WaitableMonitor(T t)
      : Monitor<Notifyable<T>>{Notifyable<T>{cv_, std::move(t)}} {}

  /**
   * wait_for time or pred is true.
   *
   * @param rel_time time to wait max
   * @param pred invocable that receives the monitored T
   */
  template <class Rep, class Period, class Pred>
  auto wait_for(const std::chrono::duration<Rep, Period> &rel_time, Pred pred) {
    std::unique_lock<std::mutex> lk{this->mtx_};

    return cv_.wait_for(lk, rel_time,
                        [this, pred]() { return pred(this->t_); });
  }

 private:
  std::condition_variable cv_;
};

#endif
