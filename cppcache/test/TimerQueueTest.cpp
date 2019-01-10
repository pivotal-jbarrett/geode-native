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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

#include <gtest/gtest.h>

#include "TimerQueue.hpp"

using apache::geode::client::TimerQueue;

class TestableTimerEvent {
 private:
  std::vector<TestableTimerEvent*>& order_;
  std::mutex& mutex_;

 public:
  std::condition_variable condition_;
  bool called_;

  inline TestableTimerEvent(std::vector<TestableTimerEvent*>& order,
                            std::mutex& mutex)
      : order_(order), mutex_(mutex), called_(false) {}

  inline void operator()() {
    std::lock_guard<std::mutex> lock(mutex_);
    order_.push_back(this);
    called_ = true;
    condition_.notify_all();
  }

  template <class... Args>
  inline bool wait_for(std::chrono::duration<Args...> duration) {
    std::unique_lock<std::mutex> lock(mutex_);
    return condition_.wait_for(lock, duration, [&] { return called_; });
  }
};

class clickity_clock {
 public:
  typedef std::chrono::nanoseconds duration;
  typedef duration::rep rep;
  typedef duration::period period;
  typedef std::chrono::time_point<clickity_clock, duration> time_point;
  static constexpr const bool is_steady = true;

  static time_point now() noexcept { return time_point(clicks_); }

  static void click() { clicks_++; }

 private:
  static duration clicks_;
};

clickity_clock::duration clickity_clock::clicks_ = std::chrono::nanoseconds(0);

TEST(TimerQueueTest, oneEventSpecficTime) {
  TimerQueue<clickity_clock> timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event(order, mutex);
  auto id = timerQueue.schedule(
      clickity_clock::now() + std::chrono::nanoseconds(2), [&] { event(); });
  EXPECT_EQ(1, id);

  clickity_clock::click();
  auto called = event.wait_for(std::chrono::seconds(1));
  EXPECT_FALSE(called);

  clickity_clock::click();
  called = event.wait_for(std::chrono::seconds(1));
  EXPECT_TRUE(called);

  EXPECT_EQ(std::vector<TestableTimerEvent*>({&event}), order);
}

TEST(TimerQueueTest, oneEventByDruation) {
  TimerQueue<clickity_clock> timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event(order, mutex);
  auto id = timerQueue.schedule(std::chrono::nanoseconds(2), [&] { event(); });
  EXPECT_EQ(1, id);

  clickity_clock::click();
  auto called = event.wait_for(std::chrono::seconds(1));
  EXPECT_FALSE(called);

  clickity_clock::click();
  called = event.wait_for(std::chrono::seconds(1));
  EXPECT_TRUE(called);

  EXPECT_EQ(std::vector<TestableTimerEvent*>({&event}), order);
}

TEST(TimerQueueTest, twoEventsAddedInOrder) {
  TimerQueue<clickity_clock> timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event1(order, mutex);
  auto id1 =
      timerQueue.schedule(std::chrono::nanoseconds(1), [&] { event1(); });
  EXPECT_EQ(1, id1);

  TestableTimerEvent event2(order, mutex);
  auto id2 =
      timerQueue.schedule(std::chrono::nanoseconds(2), [&] { event2(); });
  EXPECT_EQ(2, id2);

  clickity_clock::click();
  auto called1 = event1.wait_for(std::chrono::seconds(1));
  EXPECT_TRUE(called1);
  auto called2 = event2.wait_for(std::chrono::seconds(1));
  EXPECT_FALSE(called2);

  clickity_clock::click();
  called2 = event2.wait_for(std::chrono::seconds(1));
  EXPECT_TRUE(called2);

  EXPECT_EQ(std::vector<TestableTimerEvent*>({&event1, &event2}), order);
}

TEST(TimerQueueTest, twoEventsAddedOutOfOrder) {
  TimerQueue<clickity_clock> timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event2(order, mutex);
  auto id2 =
      timerQueue.schedule(std::chrono::nanoseconds(2), [&] { event2(); });
  EXPECT_EQ(1, id2);

  TestableTimerEvent event1(order, mutex);
  auto id1 =
      timerQueue.schedule(std::chrono::nanoseconds(1), [&] { event1(); });
  EXPECT_EQ(2, id1);

  clickity_clock::click();
  auto called1 = event1.wait_for(std::chrono::seconds(1));
  EXPECT_TRUE(called1);
  auto called2 = event2.wait_for(std::chrono::seconds(1));
  EXPECT_FALSE(called2);

  clickity_clock::click();
  called2 = event2.wait_for(std::chrono::seconds(1));
  EXPECT_TRUE(called2);

  EXPECT_EQ(std::vector<TestableTimerEvent*>({&event1, &event2}), order);
}

TEST(TimerQueueTest, cancelTimer) {
  TimerQueue<clickity_clock> timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event(order, mutex);
  auto id = timerQueue.schedule(std::chrono::nanoseconds(1), [&] { event(); });
  EXPECT_EQ(1, id);

  auto canceled = timerQueue.cancel(id);
  EXPECT_TRUE(canceled);

  clickity_clock::click();
  auto called = event.wait_for(std::chrono::seconds(1));
  EXPECT_FALSE(called);

  EXPECT_EQ(std::vector<TestableTimerEvent*>({}), order);
}

TEST(TimerQueueTest, cancelTimeNextEventFires) {
  TimerQueue<clickity_clock> timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event1(order, mutex);
  auto id1 =
      timerQueue.schedule(std::chrono::nanoseconds(1), [&] { event1(); });
  EXPECT_EQ(1, id1);

  TestableTimerEvent event2(order, mutex);
  auto id2 =
      timerQueue.schedule(std::chrono::nanoseconds(2), [&] { event2(); });
  EXPECT_EQ(2, id2);

  auto canceled = timerQueue.cancel(id1);
  EXPECT_TRUE(canceled);

  clickity_clock::click();
  auto called1 = event1.wait_for(std::chrono::seconds(1));
  EXPECT_FALSE(called1);

  clickity_clock::click();
  auto called2 = event2.wait_for(std::chrono::seconds(1));
  EXPECT_TRUE(called2);

  EXPECT_EQ(std::vector<TestableTimerEvent*>({&event2}), order);
}
