# Arduino Project: MIDI-DMX Bridge

![CI](https://github.com/chrisneukam/arduino-MidiDmxBridge/actions/workflows/cmake-ubuntu-latest.yml/badge.svg)
![Arduino Checks](https://github.com/chrisneukam/arduino-MidiDmxBridge/actions/workflows/arduino-checks.yml/badge.svg)
![Spell Check](https://github.com/chrisneukam/arduino-MidiDmxBridge/actions/workflows/spell-check.yml/badge.svg)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

The `MIDI-DMX Bridge` is an Arduino-based hardware and software solution that converts MIDI Continuous Controller signals to DMX commands. The project consists of a hardware schematic and PCB designed using KiCad, an Arduino sketch, and an Arduino library. It can be used to control DMX fixtures such as lights, fog machines, and more.

## Lightning Fixture Compatibility

The `MIDI-DMX Bridge` Arduino project is designed to seamlessly integrate with the lighting fixtures *Stairville CLB4 RGB Compact LED Bar 4* and two *Stairville LED Flood Panel 150*. Here's how the bridge supports these fixtures:

1. **Stairville CLB4 RGB Compact LED Bar 4**:
    - This LED bar operates in the 14-channel DMX mode, the start address 1.
    - Each of its four individual LED spots can be controlled independently using the MIDI-DMX Bridge.
    - Adjust colors, intensity, and effects with precision.
    - Perfect for creating dynamic lighting displays in various settings.

2. **Stairville LED Flood Panel 150**:
    - The LED Flood Panel operates in the 4-channel DMX mode, the start addresses are 15 and 19.
    - Use the MIDI-DMX Bridge to orchestrate its 150 LEDs for wide-area illumination.
    - Whether it’s stage lighting, architectural displays, or events, this panel delivers impressive results.


Remember to consult the user manuals for both fixtures to configure the MIDI-DMX Bridge effectively. Enjoy your creative lighting setups! 🌟

## Hardware Schematic

The hardware schematic for this project was designed using KiCad. The schematic includes the following components:

- **MIDI Input**: This component receives MIDI Continuous Controller signals on a defined MIDI channel from an external device.
- **Arduino Board**: This component processes the incoming MIDI signals and generates DMX commands.
- **DMX Output**: This component sends DMX commands to an external DMX device.
- **Potentiometer**: This component allows the user to adjust the intensity of the DMX signal.
- **Hardware Switch**: This component allows the user to select either a predefined DMX scene or the MIDI controlled dynamic scene.

![Hardware Schematic](./gfx/midi-dmx_bridge_schematic.png)

The DMX circuit is based on the `DMX shield` from Matthias Hertel, see [DMX Shield for Arduino with isolation](https://www.mathertel.de/Arduino/DMXShield.aspx).

## PCB Design of the MIDI-DMX Bridge

The hardware design focuses on efficiency and space-saving. The compact printed circuit board (PCB) houses all essential components, including the Arduino ProMini microcontroller, MIDI interface, and DMX driver.

The PCB has the following features:

- A MIDI input jack that accepts standard 5-pin DIN connectors
- A MIDI through jack that accepts standard 5-pin DIN connectors
- A DMX output jack that supports both 3-pin XLR connectors
- A power jack that provides 9V DC power
- A Switch that selects between two DMX modes: static and dynamic scene
- A potentiometer that adjusts the DMX signal strength

The PCB layout and the circuit diagram as a 3D simulation, before and after final assembly, are shown below:

<img src="./gfx/midi-dmx_bridge_pcb_simulation.png" alt="PCB 3D layout" height="256"/>

<img src="./gfx/midi-dmx_brigde_pcb.png" alt="PCB layout" height="256"/>
<img src="./gfx/midi-dmx_brigde_pcb_assembled.png" alt="PCB layout after assembly" height="256"/>

The PCB can be ordered from any PCB manufacturer that supports the KiCad format. Alternatively, the [Interactive BOM](./kicad/bom/ibom.html) tool can help with the assembly of the PCB.

The PCB shown was manufactured by [JLCPCB](https://jlcpcb.com/), the production data can be found in [kicad\production](./kicad/production) as a reference.

## Arduino Sketch

The Arduino sketch for this project is responsible for processing the incoming MIDI signals and generating DMX commands. The sketch uses its own implementation to receive MIDI signals and the [DMXSerial Library](https://github.com/mathertel/DMXSerial) to send DMX commands.

```c++
#include <DMXSerial.h>
#include <MidiDmxBridge.h>

static void onDmxChange(const uint8_t channel, const uint8_t value) {
  DMXSerial.write(channel, value);
}

static SerialReaderDefault reader;
static MidiDmxBridge MDXBridge(kMidiChannel, onDmxChange, reader);

void setup() {
  DMXSerial.init(DMXController);
  MDXBridge.begin();
}

void loop() {
  MDXBridge.setAttenuation(analogRead(kSensorPin));
  MDXBridge.listen();
}
```

For the complete Arduino sketch, please refer to [arduino-MidiDmxBridge.ino](./src/arduino-MidiDmxBridge/arduino-MidiDmxBridge.ino).

## Arduino Library

The Arduino library for this project is a custom library that provides additional functionality for processing MIDI signals and generating DMX commands. It is also available as [MidiDmxBridge](https://reference.arduino.cc/reference/en/libraries/mididmxbridge/) via the [Arduino Library manager](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries/).
The library includes the following functions:

- `void begin()`: This function processes initializes the library.
- `void setStaticScene(const DmxRgbChannels& channels, const DmxRgb& rgb)`: This function defines the DMX static scene
- `void setAttenuation(const uint16_t attenuation)`: This function switches to the MIDI controlled dynamic scene.
- `void switchToDynamicScene()`: This function sets the attenuation of the generated DMX signal.
- `void switchToStaticScene()`: This function switches to the predefined static scene.
- `void listen()`: This function listens on the serial interface for the next MIDI CC value and updates the DMX state.

```c++
class MidiDmxBridge {
public:
  MidiDmxBridge(const uint8_t channel, DmxOnChangeCallback callback, ISerialReader& serial);
  void begin();
  void setStaticScene(const DmxRgbChannels& channels, const DmxRgb& rgb);
  void setAttenuation(const uint16_t attenuation);
  void switchToDynamicScene();
  void switchToStaticScene();
  void listen();
};
```

For the complete API, please refer to [MidiDmxBridge.h](./src/arduino-library/MidiDmxBridge/src/MidiDmxBridge.h).

### C++ Library development

The Arduino library is developed in C++ and uses CMake for configuration. The library is tested using gtest and documented using Doxygen.

```shell
cmake -S ./src/arduino-library/ -B ./build
cmake --build ./build
ctest --test-dir ./build/
```

On Linux OS code coverage using lcov and gcov is available:

```shell
cmake -DENABLE_COVERAGE=ON -S ./src/arduino-library/ -B ./build
cmake --build ./build
ctest --test-dir ./build/
cmake --build ./build --target coverage
```

The software documentation can then be accessed at [./build/html/index.html](./build/html/index.html), the code coverage results at [./build/coverage/index.html](./build/coverage/index.html).

## How to Run Codespell

To run [codespell](https://github.com/codespell-project/codespell), you need to have Python 3 installed on your system. You can install Python 3 from the official website: https://www.python.org/downloads/.

Once you have Python 3 installed, you can install [codespell](https://github.com/codespell-project/codespell) using pip, the Python package manager. Open a terminal or command prompt and type the following command:

```bash
pip install codespell
```

This will install [codespell](https://github.com/codespell-project/codespell) and all its dependencies.

To run [codespell](https://github.com/codespell-project/codespell), navigate to the root directory of your project in the terminal or command prompt and type the following command:

```bash
codespell
```

This will run [codespell](https://github.com/codespell-project/codespell) on all the files in the current directory and its subdirectories. [codespell](https://github.com/codespell-project/codespell) will check for common misspellings and suggest corrections. You can use the `-w` option to write the changes directly to the files.

```bash
codespell -w
```

For more information on how to use [codespell](https://github.com/codespell-project/codespell), including a list of all the available options, please refer to the official documentation: https://github.com/codespell-project/codespell#usage.

## Generating a Source Code Release Package with CPack

CPack is a powerful tool that allows to create binary installers and source packages. To generate a source code release package, use the following commands:

```bash
cd ./build
cpack --config CPackSourceConfig.cmake
```

This will create a source package in the CMake build directory.

## Conclusion

This project provides a simple and effective solution for converting MIDI Continuous Controller signals to DMX commands. The hardware schematic, Arduino sketch, and Arduino library can be easily modified to suit your specific needs. If you have any questions or comments about this project, please feel free to contact me.

## License

This library and the PCB design are released under the Apache V2.0 License. See [LICENSE](./LICENSE) for more information.

Please let me know if you have any questions or feedback. I'm happy to help! 😊🎵
