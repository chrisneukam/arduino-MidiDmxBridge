/**
 * @file DmxTests.cpp
 * @author Christian Neukam
 * @brief Unit Tests for the mididmxbridge::dmx::Dmx class
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

#include <functional>

#include "Dmx.h"

namespace mididmxbridge::unittest {
using namespace mididmxbridge::dmx;

using std::placeholders::_1;
using std::placeholders::_2;
using testing::_;

static const uint8_t kMidiMaxValue = 0x7f;  /**< maximum allowed MIDI CC value (127) */
static const uint8_t kDmxMaxValue = 0xfe;   /**< maximum possible DMX value (254) */
static const uint16_t kGainMaxValue = 1024; /**< maximum allowed gain value */
static const uint16_t kGainDeadZone = 5;    /**< the gain dead zone */

/**
 * @brief This class provides the fixture for the Test Suite, which checks the DMX class.
 *
 */
class DmxTestSuite : public testing::Test {
 public:
  /**
   * @brief Construct a new DmxTestSuite object.
   *
   */
  DmxTestSuite() : mDut(std::bind(&DmxTestSuite::onChangeCallback, this, _1, _2)) {
    mDmxRgbChannels.red.push_back(1);    // assign DMX channel 1 to red
    mDmxRgbChannels.green.push_back(2);  // assign DMX channel 2 to green
    mDmxRgbChannels.blue.push_back(3);   // assign DMX channel 3 to blue
  }

  /**
   * @brief Mock method for mididmxbridge::dmx::DmxOnChangeCallback.
   *
   */
  MOCK_METHOD(void, onChangeCallback, (const uint8_t channel, const uint8_t value), ());

 protected:
  const DmxRgb mDmxRgb = {21, 42, 63}; /**< the static RGB scene to use */
  DmxRgbChannels mDmxRgbChannels;      /**< the DMX channels to associate with the static scene */
  Dmx mDut;                            /**< the device under test */
};

class DmxValueTestSuite : public DmxTestSuite,
                          public testing::WithParamInterface<std::tuple<uint8_t, uint8_t>> {};
class MidiCcValueTestSuite : public DmxValueTestSuite {};
class DmxGainTestSuite : public DmxTestSuite, public testing::WithParamInterface<uint16_t> {};
class DmxGainOutsideDeadZoneTestSuite : public DmxGainTestSuite {};
class DmxGainInsideDeadZoneTestSuite : public DmxGainTestSuite {};

/**
 * @brief This test suite checks whether a valid DMX parameter triggers a
 * mididmxbridge::dmx::DmxOnChangeCallback with a DMX value pair.
 *
 * The test design is based on boundary-value analysis with the following equivalence
 * groups:
 *
 * | channel range  | description |
 * | -------------- | ----------- |
 * | (-inf, 0)      | not required -> input range is unsigned |
 * | [0, 1]         | lower boundary, valid input |
 * | [126, 127]     | upper boundary, valid input |
 * | [128, 255]     | upper boundary, invalid input, gets clipped to 127 |
 *
 * | value range  | description |
 * | -------------- | ----------- |
 * | (-inf, 0)      | not required -> input range is unsigned |
 * | [0, 1]         | lower boundary, valid input |
 * | [254, 255]     | upper boundary, valid input |
 *
 * @see DmxValueTestSuite
 */
INSTANTIATE_TEST_SUITE_P(DMX, DmxValueTestSuite,
                         testing::Combine(testing::Values(0, 1, kMidiMaxValue - 1, kMidiMaxValue,
                                                          kMidiMaxValue + 1, 254, 255),
                                          testing::Values(0, 1, 254, 255)),
                         [](const testing::TestParamInfo<DmxValueTestSuite::ParamType>& info) {
                           std::string name = std::to_string(std::get<0>(info.param)) + "_" +
                                              std::to_string(std::get<1>(info.param));
                           return name;
                         });

/**
 * @brief This test suite checks whether a valid DMX parameter triggers a
 * mididmxbridge::dmx::DmxOnChangeCallback with a MIDI CC value pair.
 *
 * The test design is based on boundary-value analysis with the following equivalence
 * groups:
 *
 * | channel range  | description |
 * | -------------- | ----------- |
 * | (-inf, 0)      | not required -> input range is unsigned |
 * | [0, 1]         | lower boundary, valid input |
 * | [126, 127]     | upper boundary, valid input |
 * | [128, 255]     | upper boundary, invalid input, gets clipped to 127 |
 *
 * | value range  | description |
 * | -------------- | ----------- |
 * | (-inf, 0)      | not required -> input range is unsigned |
 * | [0, 1]         | lower boundary, valid input |
 * | [126, 127]     | upper boundary, valid input |
 * | [128, 255]     | upper boundary, invalid input, gets clipped to 127 |
 *
 * @see MidiCcValueTestSuite
 */
INSTANTIATE_TEST_SUITE_P(DMX, MidiCcValueTestSuite,
                         testing::Combine(testing::Values(0, 1, kMidiMaxValue - 1, kMidiMaxValue,
                                                          kMidiMaxValue + 1, 254, 255),
                                          testing::Values(0, 1, kMidiMaxValue - 1, kMidiMaxValue,
                                                          kMidiMaxValue + 1, 254, 255)),
                         [](const testing::TestParamInfo<MidiCcValueTestSuite::ParamType>& info) {
                           std::string name = std::to_string(std::get<0>(info.param)) + "_" +
                                              std::to_string(std::get<1>(info.param));
                           return name;
                         });

/**
 * @brief This test suite checks the attenuation of DMX control signals.
 *
 * @note A gain value of 1024 means unity gain, values lower than this mean a reduction.
 * Values greater than 1024 are not permitted and are clipped to 1024.
 *
 * The test design is based on boundary-value analysis with the following equivalence
 * groups:
 *
 * | value range  | description |
 * | ------------ | ----------- |
 * | (-inf, 0)    | not required -> input range is unsigned |
 * | [0, 1]       | lower boundary, valid input |
 * | [1023, 1024] | upper boundary, valid input |
 * | [1025, inf)  | invalid value range, triggers clipping |
 *
 * @see DmxGainTestSuite
 */
INSTANTIATE_TEST_SUITE_P(DMX, DmxGainTestSuite,
                         testing::Values(0, 1, kGainMaxValue - 1, kGainMaxValue, kGainMaxValue + 1,
                                         32767),
                         [](const testing::TestParamInfo<DmxGainTestSuite::ParamType>& info) {
                           return std::to_string(info.param);
                         });

/**
 * @brief This test suite checks the attenuation of DMX control signals in case the gain is outside
 * the dead zone.
 *
 * @note The dead zone of the gain values is defined in the range of [-1, 1] around the current gain
 * value.
 *
 * The test design is based on boundary-value analysis with the following equivalence
 * groups:
 *
 * | gain range   | description |
 * | ------------ | ----------- |
 * | [0, 1]       | lower boundary, will trigger a DMX gain update |
 * | [1023, 1024] | upper boundary, will trigger a DMX gain update |
 *
 * @see DmxGainOutsideDeadZoneTestSuite
 */
INSTANTIATE_TEST_SUITE_P(
    DMX, DmxGainOutsideDeadZoneTestSuite,
    testing::Values(0, 1, 509, 510, 511, 512, 513, 514, 515, 1023, 1024),
    [](const testing::TestParamInfo<DmxGainOutsideDeadZoneTestSuite::ParamType>& info) {
      return std::to_string(info.param);
    });

/**
 * @brief This test suite checks the attenuation of DMX control signals in case the gain is outside
 * the dead zone.
 *
 * @note The dead zone of the gain values is defined in the range of [-1, 1] around the current gain
 * value.
 *
 * @note The values 510 and 514 are not explicitly checked as they result in the same values as the
 * boundary gain values due to rounding effects.
 *
 * The test design is based on boundary-value analysis with the following equivalence
 * groups:
 *
 * | gain range   | description |
 * | ------------ | ----------- |
 * | [0, 506]     | outside dead zone, will trigger a callback |
 * | [507, 511]   | gain inside the lower dead zone assuming a pre gain of 512 |
 * | [513, 517]   | gain inside the upper dead zone assuming a pre gain of 512 |
 * | [518, 1024]  | outside dead zone, will trigger a callback |
 *
 * @see DmxGainInsideDeadZoneTestSuite
 */
INSTANTIATE_TEST_SUITE_P(
    DMX, DmxGainInsideDeadZoneTestSuite,
    testing::Values(500, 506, 507, 511, 512, 513, 517, 518, 600),
    [](const testing::TestParamInfo<DmxGainInsideDeadZoneTestSuite::ParamType>& info) {
      return std::to_string(info.param);
    });

/**
 * @brief This test case checks whether the function mididmxbridge::dmx::Dmx::setDmxValue()
 * does not trigger a mididmxbridge::dmx::DmxOnChangeCallback in case the
 * mididmxbridge::dmx::DmxValue is invalid.
 *
 */
TEST_F(DmxTestSuite, setDmxValue_invalid_doesNot_triggers_callback) {
  EXPECT_CALL(*this, onChangeCallback(_, _)).Times(0);
  mDut.setDmxValue({});
}

/**
 * @brief This test case checks whether the function mididmxbridge::dmx::Dmx::setDmxValue()
 * triggers a mididmxbridge::dmx::DmxOnChangeCallback in case the mididmxbridge::dmx::DmxValue is
 * valid.
 *
 * @note If the channel parameter is outside the valid MIDI range, no
 * mididmxbridge::dmx::DmxOnChangeCallback callback is triggered.
 *
 */
TEST_P(DmxValueTestSuite, setDmxValue_valid_triggers_callback) {
  const auto [channel, value] = GetParam();

  if (channel <= kMidiMaxValue) {
    EXPECT_CALL(*this, onChangeCallback(channel, value)).Times(1);
  } else {
    EXPECT_CALL(*this, onChangeCallback(channel, value)).Times(0);
  }

  mDut.setDmxValue({channel, value});
}

/**
 * @brief This test case checks whether the function mididmxbridge::dmx::Dmx::setMidiCcValue()
 * triggers a mididmxbridge::dmx::DmxOnChangeCallback in case the MIDI CC parameters are in the
 * valid range.
 *
 * @note If the channel or value parameter is outside the valid MIDI range, it is clipped by
 * mididmxbridge::midi::ContinuousController.
 *
 */
TEST_P(MidiCcValueTestSuite, setMidiCcValue_triggers_callback) {
  const auto [channel, value] = GetParam();
  const uint8_t dmxValue = (value > kMidiMaxValue) ? kDmxMaxValue : value * 2;
  const uint8_t dmxChannel = std::min(channel, kMidiMaxValue);

  EXPECT_CALL(*this, onChangeCallback(dmxChannel, dmxValue)).Times(1);
  mDut.setMidiCcValue(channel, value);
}

/**
 * @brief This test case checks whether the function mididmxbridge::dmx::Dmx::setGain()
 * applies a gain value to the resulting DMX signal, where 1024 means unity gain.
 *
 */
TEST_P(DmxGainTestSuite, setGain_triggers_callback) {
  const auto gain = GetParam();
  const uint16_t gainInit = (gain + kGainMaxValue * 3 / 4) % kGainMaxValue;
  const uint8_t dmxValue = 255u;
  const uint8_t dmxValueGainInit = (dmxValue * gainInit) / kGainMaxValue;
  const uint8_t dmxValueGain = (dmxValue * gain) / kGainMaxValue;

  if (gain >= kGainMaxValue) {
    EXPECT_CALL(*this, onChangeCallback(0, dmxValue));
  } else {
    EXPECT_CALL(*this, onChangeCallback(0, dmxValueGain));
  }
  EXPECT_CALL(*this, onChangeCallback(0, dmxValueGainInit));

  mDut.setGain(gainInit);
  mDut.setDmxValue({0, dmxValue});
  mDut.setGain(gain);
}

/**
 * @brief This test case checks whether the mididmxbridge::dmx::Dmx::setGain() function applies a
 * gain value to the resulting DMX signal if the gain is outside the dead zone.
 *
 */
TEST_P(DmxGainOutsideDeadZoneTestSuite, setGain_outside_deadZone_triggers_callback) {
  const auto gain = GetParam();
  const uint16_t gainInit = (gain + kGainMaxValue * 3 / 4) % kGainMaxValue;
  const uint8_t dmxValue = 254;
  const uint8_t dmxValueGainInit = (dmxValue * gainInit) / kGainMaxValue;
  const uint8_t dmxValueGain = (dmxValue * gain) / kGainMaxValue;

  EXPECT_CALL(*this, onChangeCallback(0, dmxValueGainInit));
  EXPECT_CALL(*this, onChangeCallback(0, dmxValueGain));

  mDut.setGain(gainInit);
  mDut.setDmxValue({0, dmxValue});
  mDut.setGain(gain);
}

/**
 * @brief This test case checks whether the mididmxbridge::dmx::Dmx::setGain() function applies a
 * gain value to the resulting DMX signal if the gain is outside the dead zone.
 *
 */
TEST_P(DmxGainInsideDeadZoneTestSuite, setGain_inside_deadZone_dont_triggers_callback) {
  const auto gain = GetParam();
  const uint16_t gainInit = 512;
  const uint8_t dmxValue = 254;
  const uint8_t dmxValueGainInit = (dmxValue * gainInit) / kGainMaxValue;
  const uint8_t dmxValueGain = (dmxValue * gain) / kGainMaxValue;

  EXPECT_CALL(*this, onChangeCallback(0, dmxValue));
  EXPECT_CALL(*this, onChangeCallback(0, dmxValueGainInit));

  if ((std::max(gain, gainInit) - std::min(gain, gainInit)) > 5) {
    // The third callback is only triggered if the last gain is outside the dead zone.
    // The dead zone is on the range [-5, 5] around the current gain.
    EXPECT_CALL(*this, onChangeCallback(0, dmxValueGain));
  }

  mDut.setDmxValue({0, dmxValue});
  mDut.setGain(gainInit);
  mDut.setGain(gain);
}

/**
 * @brief This test case checks whether activating the static scene calls the corresponding
 * mididmxbridge::dmx::DmxOnChangeCallback callbacks.
 *
 */
TEST_F(DmxTestSuite, activateStaticScene_triggers_callback_with_static_scene) {
  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.red[0], mDmxRgb.red));
  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.green[0], mDmxRgb.green));
  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.blue[0], mDmxRgb.blue));

  mDut.setStaticScene(mDmxRgbChannels, mDmxRgb);
  mDut.activateStaticScene();
}

/**
 * @brief This test case checks whether no mididmxbridge::dmx::DmxOnChangeCallback callbacks are
 * called for the dynamic scene if the static scene is active.
 *
 */
TEST_F(DmxTestSuite, activateStaticScene_prevents_callback_with_dynamic_scene) {
  EXPECT_CALL(*this, onChangeCallback(_, _)).Times(0);

  mDut.activateStaticScene();
  mDut.setDmxValue({1, 42});
}

/**
 * @brief This test case checks whether a blackout is executed on the dynamic scene when the static
 * scene is activated.
 *
 */
TEST_F(DmxTestSuite, activateStaticScene_triggers_blackout_for_dynamic_scene) {
  testing::InSequence s;

  EXPECT_CALL(*this, onChangeCallback(1, 42));
  EXPECT_CALL(*this, onChangeCallback(1, 0));

  mDut.setDmxValue({1, 42});
  mDut.activateStaticScene();
}

/**
 * @brief This test case checks whether activating the dynamic scene calls the corresponding
 * mididmxbridge::dmx::DmxOnChangeCallback callbacks.
 *
 */
TEST_F(DmxTestSuite, activateDynamicScene_triggers_callback_with_dynamic_scene) {
  testing::InSequence s;

  EXPECT_CALL(*this, onChangeCallback(1, 42));
  EXPECT_CALL(*this, onChangeCallback(1, 0));
  EXPECT_CALL(*this, onChangeCallback(1, 42));

  mDut.setDmxValue({1, 42});
  mDut.activateStaticScene();
  mDut.activateDynamicScene();
}

/**
 * @brief This test case checks whether a blackout is executed on the static scene when the dynamic
 * scene is activated.
 *
 */
TEST_F(DmxTestSuite, activateDynamicScene_triggers_blackout_for_static_scene) {
  testing::InSequence s;

  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.red[0], mDmxRgb.red));
  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.green[0], mDmxRgb.green));
  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.blue[0], mDmxRgb.blue));
  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.red[0], 0));
  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.green[0], 0));
  EXPECT_CALL(*this, onChangeCallback(mDmxRgbChannels.blue[0], 0));
  EXPECT_CALL(*this, onChangeCallback(1, 42));

  mDut.setStaticScene(mDmxRgbChannels, mDmxRgb);
  mDut.activateStaticScene();
  mDut.setDmxValue({1, 42});
  mDut.activateDynamicScene();
}
}  // namespace mididmxbridge::unittest
