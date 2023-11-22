/**
 * @file arduino-MidiDmxBridge.ino
 * @author Christian Neukam
 * @brief Arduino program of the MIDI-DMX Bridge project.
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
#include <DMXSerial.h>
#include <MidiDmxBridge.h>

#if defined(USBCON)
#define USBSerial /**< Micro and other USB boards use Serial for tracing. */
#endif

/***************************************************************/
/* configuration - Arduino Pro / Pro Mini only !!              */
/***************************************************************/
#define kMidiChannel 9 /**< the MIDI channel to listen to */
#define kSensorPin A2  /**< the input pin for the potentiometer */
#define kMidiRxPin 2   /**< the input pin for the MIDI data */
#define kMidiTxPin 3   /**< the output pin for the MIDI data (not used) */
#define kButtonPin 4   /**< the input pin for the hardware switch */

/**
 * @brief Implementation of the callback mididmxbridge::dmx::DmxOnChangeCallback.
 *
 * This callback is always called as soon as a new DMX date is generated from a
 * MIDI CC signal.
 *
 * @param[in] channel the DMX channel in the range [1, 255]
 * @param[in] value the DMX value in the range [1, 255]
 */
static void onDmxChange(const uint8_t channel, const uint8_t value) {
  DMXSerial.write(channel, value);

#ifdef USBSerial
  String msg = "DMX channel / value: [" + String(channel) + " | " + String(value) + "]";
  Serial.println(msg);
#endif
}

static SerialReaderDefault reader(kMidiRxPin, kMidiTxPin); /**< the serial reader receiving MIDI */
static MidiDmxBridge MDXBridge(kMidiChannel, onDmxChange, reader); /**< the MidiDmxBridge object */

/**
 * @brief Setup the Arduino board.
 *
 * The setup routine is called up once when the Arduino board is started. The
 * setup routine is called up once when the Arduino board is started. This is
 * where the serial interfaces are initialized and the pin modes are set.
 *
 * The static scene is calibrated to warm white with the color code for 5100K
 * (rgb = [255, 248, 167] = [ff, f8, a7]).
 */
void setup() {
  pinMode(kButtonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  uint8_t r[5] = {1, 4, 7, 10, 15};  // DMX channels of red LEDs
  uint8_t g[5] = {2, 5, 8, 11, 16};  // DMX channels of green LEDs
  uint8_t b[5] = {3, 6, 9, 12, 17};  // DMX channels of blue LEDs

#ifdef USBSerial
  Serial.begin(9600);  // print info on the serial monitor, USB only
#endif
  DMXSerial.init(DMXController);
  DMXSerial.maxChannel(128);
  MDXBridge.begin();
  MDXBridge.setStaticScene({{5, r}, {5, g}, {5, b}}, {0xff, 0xf8, 0xa7});
}

/**
 * @brief The main processing loop.
 *
 */
void loop() {
  const int state = digitalRead(kButtonPin);

  if (HIGH == state) {
    MDXBridge.switchToStaticScene();
  } else {
    MDXBridge.switchToDynamicScene();
  }
  digitalWrite(LED_BUILTIN, state);

  MDXBridge.setAttenuation(analogRead(kSensorPin));
  MDXBridge.listen();
}
