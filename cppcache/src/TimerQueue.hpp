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

struct TimerCompare;

class TimerEntry {
 public:
  typedef uint64_t id_type;
  typedef std::chrono::steady_clock clock;
  typedef typename clock::time_point time_point;
  typedef std::packaged_task<void(void)> packaged_task;

  inline TimerEntry(id_type id, time_point when, packaged_task&& task)
      : id_(id),
        when_(when),
        task_(std::make_shared<packaged_task>(std::move(task))) {}

  inline TimerEntry(const TimerEntry& from, time_point when)
      : id_(from.id_), when_(when), task_(from.task_) {}

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

  friend TimerCompare;
};

struct TimerCompare {
  inline bool operator()(const TimerEntry& lhs, const TimerEntry& rhs) const {
    return lhs.when_ > rhs.when_;
  }
};

class TimerQueue
    : private std::priority_queue<TimerEntry, std::vector<TimerEntry>,
                                  TimerCompare> {
 public:
  typedef uint64_t id_type;
  typedef std::chrono::steady_clock clock;
  typedef typename clock::time_point time_point;
  typedef std::packaged_task<void(void)> packaged_task;

  TimerQueue();

  inline ~TimerQueue() noexcept { stop(); }

  template <class Function>
  inline id_type schedule(time_point when, Function&& task) {
    return schedule(when,
                    packaged_task(std::bind(std::forward<Function>(task))));
  }

  id_type schedule(const time_point& when, packaged_task&& packagedTask);

  template <class... Args, class Function>
  inline id_type schedule(std::chrono::duration<Args...> duration,
                          Function&& task) {
    return schedule(time_point::clock::now() + duration,
                    std::forward<Function>(task));
  }

  bool reschedule(id_type id, time_point when);

  template <class... Args>
  inline bool reschedule(id_type id, std::chrono::duration<Args...> duration) {
    return reschedule(id, time_point::clock::now() + duration);
  }

  bool cancel(typename TimerEntry::id_type id);

 protected:
  void stop();

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
