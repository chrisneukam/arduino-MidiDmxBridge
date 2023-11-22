/**
 * @file DmxValueTests.cpp
 * @author Christian Neukam
 * @brief Unit Tests for the mididmxbridge::dmx::DmxValue class
 * @version 1.0
 * @date 2023-12-28
 *
 * @copyright Copyright 2023 Christian Neukam. All rights reserved.
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

#include "DmxValue.h"

namespace mididmxbridge::unittest {
using namespace mididmxbridge::dmx;

/**
 * @brief This test case checks whether the copy constructor of the mididmxbridge::dmx::DmxValue
 * initializes a new object equal to the right-hand-side object.
 *
 */
TEST(DmxValueTestSuite, construct_copy_shall_pass) {
  DmxValue dmxValue{21, 42};

  EXPECT_EQ(dmxValue, DmxValue(dmxValue));
}

/**
 * @brief This test case checks whether the assignment operator of the mididmxbridge::dmx::DmxValue
 * initializes a new object equal to the right-hand-side object.
 *
 */
TEST(DmxValueTestSuite, construct_assignment_shall_pass) {
  DmxValue dmxValue{21, 42};

  auto dut = dmxValue;
  EXPECT_EQ(dmxValue, dut);
}

/**
 * @brief This test case checks whether the compare operators of a mididmxbridge::dmx::DmxValue
 * return the anticipated result.
 *
 */
TEST(DmxValueTestSuite, compare_operators_shall_pass) {
  DmxValue dmxValue{21, 42};

  EXPECT_TRUE(dmxValue == DmxValue(dmxValue));
  EXPECT_FALSE(dmxValue != DmxValue(dmxValue));
  EXPECT_FALSE(dmxValue == DmxValue(0, 0));
  EXPECT_TRUE(dmxValue != DmxValue(0, 0));
}

/**
 * @brief This test case checks whether the operator bool of a mididmxbridge::dmx::DmxValue returns
 * true in case the mididmxbridge::dmx::DmxValue got set.
 *
 */
TEST(DmxValueTestSuite, operator_bool_return_true_if_set) {
  DmxValue dmxValueSet{21, 42};

  EXPECT_FALSE(DmxValue());  // Default constructed object is invalid.
  EXPECT_TRUE(DmxValue({0, 0}));
  EXPECT_TRUE(DmxValue({0, 42}));
  EXPECT_TRUE(DmxValue({21, 0}));
  EXPECT_TRUE(DmxValue({21, 42}));
}
}  // namespace mididmxbridge::unittest
