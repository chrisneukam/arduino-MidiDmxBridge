/**
 * @file VectorTests.cpp
 * @author Christian Neukam
 * @brief Unit Tests for the mididmxbridge::vector class template.
 * @version 1.0
 * @date 2024-01-07
 *
 * @copyright Copyright 2024 Christian Neukam. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "vector.h"

namespace mididmxbridge::unittest {
using mididmxbridge::vector;

/**
 * @brief This test case tests whether the default constructor of mididmxbridge::vector creates an
 * empty container.
 *
 */
TEST(VectorTestSuite, construct_empty) {
  vector<int> vec;

  EXPECT_EQ(vec.size(), 0);
  EXPECT_GT(vec.capacity(), 0);
}

/**
 * @brief This test case tests whether the constructor of mididmxbridge::vector creates a container
 * with the specified size in case it is smaller than the current capacity.
 *
 */
TEST(VectorTestSuite, construct_with_size_smaller_than_capacity) {
  vector<int> vec(1);

  EXPECT_EQ(vec.size(), 1);
  EXPECT_GT(vec.capacity(), 0);
  EXPECT_LT(vec.size(), vec.capacity());
}

/**
 * @brief This test case tests whether the constructor of mididmxbridge::vector creates a container
 * with the specified size in case it is greater than the current capacity.
 *
 */
TEST(VectorTestSuite, construct_by_size_greater_than_capacity) {
  const vector<int> ref;
  const auto refCapacity = ref.capacity();
  vector<int> vec(refCapacity + 1);

  EXPECT_EQ(vec.size(), refCapacity + 1);
  EXPECT_GT(vec.capacity(), refCapacity);
  EXPECT_LT(vec.size(), vec.capacity());
}

/**
 * @brief This test case tests whether the constructor of mididmxbridge::vector creates a container
 * with the maximum size if the requested size is greater than or equal to it.
 *
 */
TEST(VectorTestSuite, construct_by_size_greater_than_max_size) {
  const vector<int> ref;
  const auto max_size = ref.max_size();
  vector<int> vec(max_size + 1);

  EXPECT_EQ(vec.size(), max_size);
  EXPECT_EQ(vec.capacity(), max_size);
}

/**
 * @brief This test case tests whether the constructor of mididmxbridge::vector creates a container
 * with the provided value array.
 *
 */
TEST(VectorTestSuite, construct_by_value) {
  const int value = 42;
  const int values[] = {value};
  vector<int> vec(1, values);

  EXPECT_EQ(vec.size(), 1);
  EXPECT_EQ(vec[0], value);
}

/**
 * @brief This test case tests whether the function mididmxbridge::vector::empty() returns true if
 * the container is empty.
 *
 */
TEST(VectorTestSuite, empty_returns_true_for_empty_vector) {
  vector<int> vec;
  EXPECT_EQ(vec.empty(), true);

  vec.push_back(666);
  vec.pop_back();
  EXPECT_EQ(vec.empty(), true);
}

/**
 * @brief This test case tests whether the function mididmxbridge::vector::empty() returns false if
 * the container is not empty.
 *
 */
TEST(VectorTestSuite, empty_returns_false_for_non_empty_vector) {
  vector<int> vec(1);

  EXPECT_EQ(vec.empty(), false);
}

/**
 * @brief This test case tests whether the mididmxbridge::push_back() function transfers the
 * provided value to the container.
 *
 */
TEST(VectorTestSuite, push_back_increases_size) {
  const int value = 666;
  vector<int> vec;
  vec.push_back(value);

  EXPECT_EQ(vec.size(), 1);
  EXPECT_EQ(vec[0], value);
}

/**
 * @brief This test case tests whether the mididmxbridge::push_back() function increases the
 * capacity of the container if the current capacity is exceeded.
 *
 */
TEST(VectorTestSuite, push_back_resize_if_capacity_is_reached) {
  vector<int> vec;
  const auto capacity = vec.capacity();

  for (uint8_t i = 0; i <= capacity; i++) {
    vec.push_back(i);
  }

  EXPECT_EQ(vec.size(), capacity + 1);
  EXPECT_GT(vec.capacity(), capacity);

  for (uint8_t i = 0; i < vec.size(); i++) {
    EXPECT_EQ(vec[i], i);
  }
}

/**
 * @brief This test case tests whether the mididmxbridge::push_back() function does not increase the
 * capacity of the container further if the maximum capacity is reached.
 *
 */
TEST(VectorTestSuite, push_back_resize_if_max_size_is_reached_nop) {
  vector<int> ref;
  const auto max_size = ref.max_size();
  vector<int> vec(max_size);

  EXPECT_EQ(vec.size(), max_size);
  EXPECT_EQ(vec.capacity(), max_size);
  vec.push_back(42);
  EXPECT_EQ(vec.size(), max_size);
  EXPECT_EQ(vec.capacity(), max_size);
}

/**
 * @brief This test case tests whether the mididmxbridge::pop_back() function reduces the size of
 * the container by one.
 *
 */
TEST(VectorTestSuite, pop_back_decreases_size) {
  const uint8_t size = 5;
  vector<int> vec(size);

  EXPECT_EQ(vec.size(), size);
  vec.pop_back();
  EXPECT_EQ(vec.size(), size - 1);
}

/**
 * @brief This test case tests whether the function mididmxbridge::pop_back() does not change the
 * container if the current size is 0.
 *
 */
TEST(VectorTestSuite, pop_back_on_empty_nop) {
  vector<int> vec;

  vec.pop_back();
  EXPECT_EQ(vec.size(), 0);
}
}  // namespace mididmxbridge::unittest
