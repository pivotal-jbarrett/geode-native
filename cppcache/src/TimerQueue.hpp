/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifndef NATIVECLIENT_TIMERQUEUE_H
#define NATIVECLIENT_TIMERQUEUE_H

#include <algorithm>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <vector>

namespace apache {
namespace geode {
namespace client {

template <class>
struct TimerCompare;

template <class Clock = std::chrono::steady_clock>
class TimerEntry {
 public:
  typedef uint64_t id_type;
  typedef Clock clock;
  typedef typename clock::time_point time_point;
  typedef std::packaged_task<void(void)> packaged_task;

  inline TimerEntry(id_type id, time_point when, packaged_task&& task)
      : id_(id),
        when_(when),
        task_(std::make_shared<packaged_task>(std::move(task))) {}

  inline id_type getId() const { return id_; }

  inline const time_point& getWhen() const { return when_; }

  inline packaged_task& getTask() const { return *task_; }

  inline bool operator==(const TimerEntry& other) const {
    return id_ == other.id_ && when_ == other.when_;
  }

  inline bool operator!=(const TimerEntry& other) const {
    return !(*this == other);
  }

 private:
  id_type id_;
  time_point when_;
  std::shared_ptr<packaged_task> task_;

  friend TimerCompare<Clock>;
};

template <class Clock = std::chrono::steady_clock>
struct TimerCompare {
  inline bool operator()(const TimerEntry<Clock>& lhs,
                         const TimerEntry<Clock>& rhs) const {
    return lhs.when_ > rhs.when_;
  }
};

template <class Clock = std::chrono::steady_clock>
class TimerQueue : private std::priority_queue<TimerEntry<Clock>,
                                               std::vector<TimerEntry<Clock>>,
                                               TimerCompare<Clock>> {
 public:
  typedef uint64_t id_type;
  typedef Clock clock;
  typedef typename clock::time_point time_point;
  typedef std::packaged_task<void(void)> packaged_task;

  inline TimerQueue() : id_(0), stop_(false) {
    thread_ = std::thread([this] {
      while (true) {
        std::unique_lock<decltype(mutex_)> lock(mutex_);

        condition_.wait(lock, [this] { return this->stop_ || !this->empty(); });
        if (stop_) {
          break;
        }

        auto nextTimer = this->top();
        if (condition_.wait_until(lock, nextTimer.getWhen(), [&] {
              return this->stop_ ||
                     (!this->empty() && this->top() != nextTimer);
            })) {
          // Next timer replaced before expiring.
          continue;
        }

        // Next/top timer has expired.
        this->pop();
        lock.unlock();

        auto& task = nextTimer.getTask();
        auto future = task.get_future();
        task();
        future.get();
      }
    });
  }

  inline ~TimerQueue() noexcept { stop(); }

  template <class Function>
  inline id_type schedule(time_point when, Function&& task) {
    id_type id;
    {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      id = ++id_;
      this->emplace(id, when,
                    packaged_task(std::bind(std::forward<Function>(task))));
    }

    condition_.notify_one();

    return id;
  }

  template <class... Args, class Function>
  inline id_type schedule(std::chrono::duration<Args...> duration,
                          Function&& task) {
    return schedule(time_point::clock::now() + duration,
                    std::forward<Function>(task));
  }

  inline bool cancel(typename TimerEntry<Clock>::id_type id) {
    std::unique_lock<decltype(mutex_)> lock(mutex_);

    const auto& found = std::find_if(
        this->c.begin(), this->c.end(),
        [&](const TimerEntry<Clock>& entry) { return entry.getId() == id; });

    if (found != this->c.end()) {
      this->c.erase(found);
      lock.unlock();
      condition_.notify_one();
      return true;
    }

    return false;
  }

 protected:
  inline void stop() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);

    if (stop_) {
      return;
    }

    stop_ = true;

    lock.unlock();

    condition_.notify_one();
    try {
      thread_.join();
    } catch (...) {
      // ignore
    }
  }

 private:
  id_type id_;
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable condition_;
  bool stop_;
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // NATIVECLIENT_TIMERQUEUE_H
