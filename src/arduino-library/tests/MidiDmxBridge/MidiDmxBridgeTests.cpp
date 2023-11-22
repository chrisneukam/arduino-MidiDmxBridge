/**
 * @file MidiDmxBridgeTests.cpp
 * @author Christian Neukam
 * @brief Unit Tests for the MidiDmxBridge class.
 * @version 1.0
 * @date 2024-01-04
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

#include "MidiDmxBridge.h"
#include "SerialReaderMock.h"

namespace mididmxbridge::unittest {
using std::placeholders::_1;
using std::placeholders::_2;
using testing::_;
using testing::NiceMock;

/**
 * @brief This class provides the fixture for the Test Suite, which checks the
 * mididmxbridge::midi::MidiReader.
 *
 */
class mididmxbridgeTestSuite : public testing::Test {
 public:
  /**
   * @brief Construct a new mididmxbridgeTestSuite object.
   *
   */
  mididmxbridgeTestSuite()
      : mChannel(1),
        mSyncByte(0xb0),
        mSerialData({mSyncByte, 0x01, 0x02, mSyncByte, 0x03}),
        mSerial(mSerialData),
        mDut(mChannel, std::bind(&mididmxbridgeTestSuite::onChangeCallback, this, _1, _2),
             mSerial) {}

  /**
   * @brief Mock method for mididmxbridge::dmx::DmxOnChangeCallback.
   *
   */
  MOCK_METHOD(void, onChangeCallback, (const uint8_t channel, const uint8_t value), ());

 protected:
  const uint8_t mChannel;                 /**< the MIDI channel to test */
  const uint8_t mSyncByte;                /**< the first MIDI CC byte */
  const std::vector<uint8_t> mSerialData; /**< the serial data to simulate */
  NiceMock<SerialReaderMock> mSerial;     /**< the serial interface mock */
  MidiDmxBridge mDut;                     /**< the device under test */
};

/**
 * @brief This test case tests whether the function mididmxbridge::begin() calls
 * mididmxbridge::ISerialReader::begin().
 *
 */
TEST_F(mididmxbridgeTestSuite, begin_calls_serial_begin) {
  EXPECT_CALL(mSerial, begin());
  mDut.begin();
}

/**
 * @brief This test case tests whether the function MidiDmxBridge::listen() triggers a
 * mididmxbridge::dmx::DmxOnChangeCallback callback if the serial data stream contains a valid MIDI
 * CC value.
 *
 */
TEST_F(mididmxbridgeTestSuite, listen_shall_trigger_callback_with_valid_serialData) {
  EXPECT_CALL(*this, onChangeCallback(mSerialData[1], mSerialData[2] << 1));

  mDut.listen();
}

/**
 * @brief This test case tests whether the function MidiDmxBridge::listen() does not trigger a
 * mididmxbridge::dmx::DmxOnChangeCallback callback if the serial data stream does not contain a
 * valid MIDI CC value.
 *
 */
TEST_F(mididmxbridgeTestSuite, listen_shall_not_trigger_callback_with_invalid_serialData) {
  EXPECT_CALL(*this, onChangeCallback(_, _)).Times(0);

  mSerial.read();  // Skip the first byte in mSerialData so that the data packet becomes invalid.
  mDut.listen();
}

/**
 * @brief This test case tests whether the function MidiDmxBridge::switchToStaticScene() triggers a
 * mididmxbridge::dmx::DmxOnChangeCallback callback with a predefined DMX scene.
 *
 */
TEST_F(mididmxbridgeTestSuite, switchToStaticScene_triggers_callback_with_static_scene) {
  const std::vector<uint8_t> channels{1, 2, 3};

  EXPECT_CALL(*this, onChangeCallback(_, _)).Times(3);  // expect 3 callbacks for r, g and b

  mDut.setStaticScene({{1, &channels[0]}, {1, &channels[1]}, {1, &channels[2]}}, {2, 4, 6});
  mDut.switchToStaticScene();
}

/**
 * @brief This test case tests whether the function MidiDmxBridge::switchToDynamicScene() triggers a
 * mididmxbridge::dmx::DmxOnChangeCallback callback with the dynamic DMX scene.
 *
 */
TEST_F(mididmxbridgeTestSuite, switchToDynamicScene_triggers_callback_with_dynamic_scene) {
  EXPECT_CALL(*this, onChangeCallback(_, _)).Times(3);  // twice the value, once the blackout

  mDut.listen();
  mDut.switchToStaticScene();
  mDut.switchToDynamicScene();
}

/**
 * @brief This test case tests whether the function MidiDmxBridge::setAttenuation() triggers a
 * mididmxbridge::dmx::DmxOnChangeCallback callback if the gain has changed.
 *
 */
TEST_F(mididmxbridgeTestSuite, setAttenuation_shall_trigger_callback) {
  const uint8_t gain = 0;
  EXPECT_CALL(*this, onChangeCallback(mSerialData[1], mSerialData[2] << 1));
  EXPECT_CALL(*this, onChangeCallback(mSerialData[1], mSerialData[2] * gain));

  mDut.listen();
  mDut.setAttenuation(gain);
}
}  // namespace mididmxbridge::unittest
