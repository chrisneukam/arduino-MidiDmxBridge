/**
 * @file UtilTests.cpp
 * @author Christian Neukam
 * @brief Unit Tests for the mididmxbridge::util methods.
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

#include <algorithm>

#include "util.h"

namespace mididmxbridge::unittest {
using namespace mididmxbridge::util;

/**
 * @brief This test case tests whether the function mididmxbridge::util::max() returns the same
 * result as std::max().
 *
 */
TEST(UtilTestSuite, max_equals_std_max) {
  int a = 1;
  int b = 2;

  EXPECT_EQ(max_t(a, b), std::max(a, b));
  EXPECT_EQ(max_t(b, a), std::max(b, a));
  EXPECT_EQ(max_t(a, a), std::max(a, a));
}

/**
 * @brief This test case tests whether the function mididmxbridge::util::min_t() returns the same
 * result as std::min().
 *
 */
TEST(UtilTestSuite, min_equals_std_min) {
  int a = 1;
  int b = 2;

  EXPECT_EQ(min_t(a, b), std::min(a, b));
  EXPECT_EQ(min_t(b, a), std::min(b, a));
  EXPECT_EQ(min_t(a, a), std::min(a, a));
}

/**
 * @brief This test case tests whether the function mididmxbridge::util::absDiff_t() returns the
 * same result as std::abs(x - y).
 *
 */
TEST(UtilTestSuite, absDiff_returns_absoluteDifference) {
  int a = 1;
  int b = 2;

  EXPECT_EQ(absDiff_t(a, b), std::abs(a - b));
  EXPECT_EQ(absDiff_t(b, a), std::abs(b - a));
  EXPECT_EQ(absDiff_t(a, a), 0);
}
}  // namespace mididmxbridge::unittest
