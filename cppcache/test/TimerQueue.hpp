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

#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <vector>

struct TimerCompare;

class TimerEntry {
 public:
  typedef uint64_t id_type;
  typedef std::chrono::steady_clock::time_point time_point;
  typedef std::packaged_task<void(void)> packaged_task;

  inline TimerEntry(id_type id, time_point when, packaged_task&& task)
      : id_(id),
        when_(when),
        task_(std::make_shared<packaged_task>(std::move(task))) {}

  inline id_type getId() const { return id_; }

  inline const time_point& getWhen() const { return when_; }

  inline const std::shared_ptr<packaged_task>& getTask() const { return task_; }

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

class TimerQueue {
 private:
  TimerEntry::id_type id_;
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable condition_;
  std::priority_queue<TimerEntry, std::vector<TimerEntry>, TimerCompare> queue_;
  std::atomic<bool> stop_;

 public:
  inline TimerQueue() : id_(0), stop_(false) {
    thread_ = std::thread([this] {
      while (true) {
        std::unique_lock<decltype(mutex_)> lock(mutex_);

        condition_.wait(lock, [this] { return stop_ || !queue_.empty(); });
        if (stop_) {
          break;
        }

        const auto& timerEntry = queue_.top();
        //        std::cout << "TimerQueue: waiting id=" << timerEntry.getId()
        //                  << ", when="
        //                  << timerEntry.getWhen().time_since_epoch().count()
        //                  << std::endl;
        if (auto status = condition_.wait_until(
                lock, timerEntry.getWhen(),
                [&] { return stop_ || queue_.top() != timerEntry; })) {
          //          std::cout << "TimerQueue: resetting next timer " <<
          //          std::endl;
          continue;
        }

        auto task = timerEntry.getTask();
        //        std::cout << "TimerQueue: executing id=" << timerEntry.getId()
        //                  << ", locked=" << lock.owns_lock()
        //                  << ", task=" << static_cast<void*>(task.get()) <<
        //                  std::endl;
        queue_.pop();
        lock.unlock();

        auto future = task->get_future();
        (*task)();
        future.get();
      }
    });
  }

  inline ~TimerQueue() noexcept { stop(); }

  template <class Function>
  inline TimerEntry::id_type schedule(TimerEntry::time_point when,
                                      Function&& task) {
    {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      queue_.emplace(
          ++id_, when,
          TimerEntry::packaged_task(std::bind(std::forward<Function>(task))));
    }

    //    std::cout << "TimerQueue: added id=" << id_
    //              << ", when=" << when.time_since_epoch().count() <<
    //              std::endl;

    condition_.notify_one();

    return id_;
  }

  template <class... Args, class Function>
  inline TimerEntry::id_type schedule(std::chrono::duration<Args...> duration,
                                      Function&& task) {
    return schedule(TimerEntry::time_point::clock::now() + duration,
                    std::forward<Function>(task));
  }

  inline void stop() {
    if (!stop_.exchange(true)) {
      condition_.notify_one();
    }
    try {
      thread_.join();
    } catch (...) {
      // ignore
    }
  }
};

#endif  // NATIVECLIENT_TIMERQUEUE_H
