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

TEST(TimerQueueTest, oneEventSpecficTime) {
  TimerQueue timerQueue;

  std::condition_variable called;

  auto id = timerQueue.schedule(
      std::chrono::steady_clock::now() + std::chrono::seconds(1),
      TimerEntry::packaged_task([&] { called.notify_all(); }));

  ASSERT_EQ(1, id);

  std::mutex mutex;
  std::unique_lock<decltype(mutex)> lock(mutex);
  auto status = called.wait_for(lock, std::chrono::seconds(10));
  EXPECT_EQ(std::cv_status::no_timeout, status);
}

TEST(TimerQueueTest, twoEventsSpecficTimeAddedInOrder) {
  TimerQueue timerQueue;

  std::vector<int> order;
  std::mutex mutex;

  std::condition_variable condition1;
  bool called1 = false;
  auto id = timerQueue.schedule(
      std::chrono::steady_clock::now() + std::chrono::seconds(1), [&] {
        std::lock_guard<decltype(mutex)> lock(mutex);
        order.push_back(1);
        called1 = true;
        condition1.notify_all();
      });
  ASSERT_EQ(1, id);

  std::condition_variable condition2;
  bool called2 = false;
  id = timerQueue.schedule(std::chrono::seconds(2), [&] {
    std::lock_guard<decltype(mutex)> lock(mutex);
    order.push_back(2);
    called2 = true;
    condition2.notify_all();
  });
  ASSERT_EQ(2, id);

  std::unique_lock<decltype(mutex)> lock(mutex);
  auto called = condition1.wait_for(lock, std::chrono::seconds(3),
                                    [&] { return called1; });
  ASSERT_TRUE(called);
  EXPECT_TRUE(called1);
  EXPECT_EQ(1, order.size());
  EXPECT_EQ(1, order[0]);

  called = condition2.wait_for(lock, std::chrono::seconds(10),
                               [&] { return called2; });
  ASSERT_TRUE(called);
  EXPECT_TRUE(called2);
  EXPECT_EQ(2, order.size());
  EXPECT_EQ(2, order[1]);
}

TEST(TimerQueueTest, twoEventsSpecficTimeAddedOutOfOrder) {
  TimerQueue timerQueue;

  std::vector<int> order;
  std::mutex mutex;

  std::condition_variable condition2;
  bool called2 = false;
  auto id = timerQueue.schedule(
      std::chrono::steady_clock::now() + std::chrono::seconds(5), [&] {
        std::lock_guard<decltype(mutex)> lock(mutex);
        order.push_back(2);
        called2 = true;
        condition2.notify_all();
      });
  ASSERT_EQ(1, id);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::condition_variable condition1;
  bool called1 = false;
  id = timerQueue.schedule(std::chrono::seconds(1), [&] {
    std::lock_guard<decltype(mutex)> lock(mutex);
    order.push_back(1);
    called1 = true;
    condition1.notify_all();
  });
  ASSERT_EQ(2, id);

  std::unique_lock<decltype(mutex)> lock(mutex);
  auto called = condition1.wait_for(lock, std::chrono::seconds(3),
                                    [&] { return called1; });
  ASSERT_TRUE(called);
  EXPECT_TRUE(called1);
  EXPECT_EQ(1, order.size());
  EXPECT_EQ(1, order[0]);

  called = condition2.wait_for(lock, std::chrono::seconds(10),
                               [&] { return called2; });
  ASSERT_TRUE(called);
  EXPECT_TRUE(called2);
  EXPECT_EQ(2, order.size());
  EXPECT_EQ(2, order[1]);
}
