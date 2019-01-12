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

#include "TimerQueue.hpp"

namespace apache {
namespace geode {
namespace client {

TimerQueue::id_type TimerQueue::schedule(
    const TimerQueue::time_point& when,
    TimerQueue::packaged_task&& packagedTask) {
  id_type id;
  {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    id = ++id_;
    emplace(id, when, std::chrono::nanoseconds::zero(),
            std::move(packagedTask));
  }

  condition_.notify_one();
  return id;
}

TimerQueue::id_type TimerQueue::_schedule(
    const TimerQueue::time_point& initial, std::chrono::nanoseconds interval,
    TimerQueue::packaged_task&& packagedTask) {
  id_type id;
  {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    id = ++id_;
    emplace(id, initial, interval, std::move(packagedTask));
  }

  condition_.notify_one();
  return id;
}

bool TimerQueue::reschedule(TimerQueue::id_type id,
                            TimerQueue::time_point when) {
  std::unique_lock<decltype(mutex_)> lock(mutex_);

  const auto& found = std::find_if(
      c.begin(), c.end(),
      [&](const TimerEntry& entry) { return entry.getId() == id; });

  if (found != c.end()) {
    auto copy = *found;
    c.erase(found);
    emplace(copy, when);
    lock.unlock();
    condition_.notify_one();
    return true;
  }

  return false;
}

bool TimerQueue::cancel(typename TimerEntry::id_type id) {
  std::unique_lock<decltype(mutex_)> lock(mutex_);

  const auto& found = std::find_if(
      c.begin(), c.end(),
      [&](const TimerEntry& entry) { return entry.getId() == id; });

  if (found != c.end()) {
    c.erase(found);
    lock.unlock();
    condition_.notify_one();
    return true;
  }

  return false;
}

void TimerQueue::stop() {
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

TimerQueue::TimerQueue() : id_(0), stop_(false) {
  thread_ = std::thread([this] {
    while (true) {
      std::unique_lock<decltype(mutex_)> lock(mutex_);

      condition_.wait(lock, [this] { return stop_ || !empty(); });
      if (stop_) {
        break;
      }

      auto nextTimer = top();
      if (condition_.wait_until(lock, nextTimer.getWhen(), [&] {
            return stop_ || empty() || top() != nextTimer;
          })) {
        // Next timer replaced before expiring.
        continue;
      }

      // Next/top timer has expired.
      pop();
      lock.unlock();

      auto& task = nextTimer.getTask();
      auto future = task.get_future();
      task();
      future.get();

      if (nextTimer.getInterval() > std::chrono::nanoseconds::zero()) {
        task.reset();
        lock.lock();
        emplace(nextTimer, clock::now() + nextTimer.getInterval());
        lock.unlock();
      }
    }
  });
}

}  // namespace client
}  // namespace geode
}  // namespace apache
