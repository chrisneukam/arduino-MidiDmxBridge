/**
 * @file ContinuousControllerTests.cpp
 * @author Christian Neukam
 * @brief Unit Tests for the mididmxbridge::midi::ContinuousController class
 * @version 1.0
 * @date 2023-12-12
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

#include "ContinuousController.h"
#include "DmxValue.h"

namespace mididmxbridge::unittest {
using mididmxbridge::dmx::DmxValue;
using mididmxbridge::midi::ContinuousController;

static const uint8_t kMidiMaxValue = 0x7f; /**< maximum allowed MIDI CC value (127) */
static const uint8_t kDmxMaxValue = 0xfe;  /**< maximum possible DMX value (254) */

/**
 * @brief This class provides the fixture for the Test Suite, which checks the MIDI Continuous
 * Controller (CC) conversion to DMX values.
 *
 */
class MidiCcInputRangeTestSuite : public testing::TestWithParam<std::tuple<uint8_t, uint8_t>> {};

/**
 * @brief This matcher checks two mididmxbridge::DmxValue objects for equality.
 *
 */
MATCHER_P(EQ, other, "") {
  return (arg.channel() == other.channel()) && (arg.value() == other.value());
}

/**
 * @brief This test suite checks the conversion of MIDI signals into DMX control signals.
 *
 * The test design is based on boundary-value analysis with the following equivalence groups:
 *
 * | MIDI \p channel range | description |
 * | --------------------- | ----------- |
 * | (-inf, 0)  | not required -> input range is unsigned |
 * | [0, 1]     | lower boundary, valid input |
 * | [126, 127] | upper boundary, valid input |
 * | [128, inf) | invalid value range, triggers clipping |
 *
 * | MIDI \p value range | description |
 * | ------------------- | ----------- |
 * | (-inf, 0)  | not required -> input range is unsigned |
 * | [0, 1]     | lower boundary, valid input |
 * | [126, 127] | upper boundary, valid input |
 * | [128, inf) | invalid value range, triggers clipping |
 *
 * @see MidiCcInputRangeTestSuite
 */
INSTANTIATE_TEST_SUITE_P(
    ContinuousController, MidiCcInputRangeTestSuite,
    testing::Combine(
        testing::Values(0, 1, kMidiMaxValue - 1, kMidiMaxValue, kMidiMaxValue + 1, 254, 255),
        testing::Values(0, 1, kMidiMaxValue - 1, kMidiMaxValue, kMidiMaxValue + 1, 254, 255)),
    [](const testing::TestParamInfo<MidiCcInputRangeTestSuite::ParamType>& info) {
      std::string name =
          std::to_string(std::get<0>(info.param)) + "_" + std::to_string(std::get<1>(info.param));
      return name;
    });

/**
 * @brief This test case checks whether the default constructor of the
 * mididmxbridge::midi::ContinuousController initializes a new object equal to
 * mididmxbridge::midi::ContinuousController(0, 0).
 *
 */
TEST(MidiCcTestSuite, construct_default) {
  ContinuousController cc;

  EXPECT_EQ(cc, ContinuousController(0, 0));
}

/**
 * @brief This test case checks whether the compare operators of a
 * mididmxbridge::midi::ContinuousController return the anticipated result.
 *
 */
TEST(MidiCcTestSuite, compare_operators_shall_pass) {
  ContinuousController cc{21, 42};

  EXPECT_TRUE(cc == ContinuousController(cc));
  EXPECT_FALSE(cc != ContinuousController(cc));
  EXPECT_FALSE(cc == ContinuousController());
  EXPECT_TRUE(cc != ContinuousController());
}

/**
 * @brief This test case checks whether the function
 * mididmxbridge::midi::ContinuousController::toDmx() converts a MIDI Continuous Controller (CC)
 * signal into a DMX signal, where the MIDI CC command corresponds to the DMX channel and the MIDI
 * CC value corresponds to the DMX value.
 *
 */
TEST_P(MidiCcInputRangeTestSuite, toDmx_scalesDmxValue_shall_pass) {
  const auto [midiChannel, midiValue] = GetParam();
  const uint8_t dmxValue = (midiValue > kMidiMaxValue) ? kDmxMaxValue : midiValue * 2;
  const uint8_t dmxChannel = std::min(midiChannel, kMidiMaxValue);
  ContinuousController dut{midiChannel, midiValue};

  EXPECT_THAT(dut.toDmx(), EQ(DmxValue{dmxChannel, dmxValue}));
}
}  // namespace mididmxbridge::unittest
