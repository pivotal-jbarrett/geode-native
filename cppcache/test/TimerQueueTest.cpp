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

TEST(TimerQueueTest, oneEventSpecficTime) {
  TimerQueue timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event(order, mutex);
  auto id = timerQueue.schedule(
      std::chrono::steady_clock::now() + std::chrono::seconds(1),
      [&] { event(); });
  EXPECT_EQ(1, id);

  auto called = event.wait_for(std::chrono::seconds(3));
  EXPECT_TRUE(called);
  ASSERT_EQ(1, order.size());
  EXPECT_EQ(&event, order[0]);
}

TEST(TimerQueueTest, oneEventByDruation) {
  TimerQueue timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event(order, mutex);
  auto id = timerQueue.schedule(std::chrono::seconds(1), [&] { event(); });
  EXPECT_EQ(1, id);

  auto called = event.wait_for(std::chrono::seconds(3));
  EXPECT_TRUE(called);
  ASSERT_EQ(1, order.size());
  EXPECT_EQ(&event, order[0]);
}

TEST(TimerQueueTest, twoEventsAddedInOrder) {
  TimerQueue timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event1(order, mutex);
  auto id1 = timerQueue.schedule(std::chrono::seconds(1), [&] { event1(); });
  EXPECT_EQ(1, id1);

  TestableTimerEvent event2(order, mutex);
  auto id2 = timerQueue.schedule(std::chrono::seconds(5), [&] { event2(); });
  EXPECT_EQ(2, id2);

  auto called1 = event1.wait_for(std::chrono::seconds(3));
  EXPECT_TRUE(called1);
  ASSERT_EQ(1, order.size());
  EXPECT_EQ(&event1, order[0]);

  auto called2 = event2.wait_for(std::chrono::seconds(10));
  EXPECT_TRUE(called2);
  ASSERT_EQ(2, order.size());
  EXPECT_EQ(&event2, order[1]);
}

TEST(TimerQueueTest, twoEventsAddedOutOfOrder) {
  TimerQueue timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event2(order, mutex);
  auto id2 = timerQueue.schedule(std::chrono::seconds(5), [&] { event2(); });
  EXPECT_EQ(1, id2);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  TestableTimerEvent event1(order, mutex);
  auto id1 = timerQueue.schedule(std::chrono::seconds(1), [&] { event1(); });
  EXPECT_EQ(2, id1);

  auto called1 = event1.wait_for(std::chrono::seconds(3));
  EXPECT_TRUE(called1);
  ASSERT_EQ(1, order.size());
  EXPECT_EQ(&event1, order[0]);

  auto called2 = event2.wait_for(std::chrono::seconds(10));
  EXPECT_TRUE(called2);
  ASSERT_EQ(2, order.size());
  EXPECT_EQ(&event2, order[1]);
}

TEST(TimerQueueTest, cancelTimer) {
  TimerQueue timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event(order, mutex);
  auto id = timerQueue.schedule(std::chrono::seconds(1), [&] { event(); });
  EXPECT_EQ(1, id);

  auto canceled = timerQueue.cancel(id);
  EXPECT_TRUE(canceled);

  auto called = event.wait_for(std::chrono::seconds(3));
  EXPECT_FALSE(called);
  EXPECT_EQ(0, order.size());
}

TEST(TimerQueueTest, cancelTimeNextEventFires) {
  TimerQueue timerQueue;

  std::vector<TestableTimerEvent*> order;
  std::mutex mutex;

  TestableTimerEvent event1(order, mutex);
  auto id1 = timerQueue.schedule(std::chrono::seconds(1), [&] { event1(); });
  EXPECT_EQ(1, id1);

  TestableTimerEvent event2(order, mutex);
  auto id2 = timerQueue.schedule(std::chrono::seconds(5), [&] { event2(); });
  EXPECT_EQ(2, id2);

  auto canceled = timerQueue.cancel(id1);
  EXPECT_TRUE(canceled);

  auto called1 = event1.wait_for(std::chrono::seconds(3));
  EXPECT_FALSE(called1);
  EXPECT_EQ(0, order.size());

  auto called2 = event2.wait_for(std::chrono::seconds(10));
  EXPECT_TRUE(called2);
  ASSERT_EQ(1, order.size());
  EXPECT_EQ(&event2, order[0]);
}