cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

###########################################################
# Setup the project
###########################################################
set(VERSION_MAJOR   1   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

project(MidiDmxBridge VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} DESCRIPTION "MIDI-to-DMX library")

###########################################################
# Setup Google's gtest/gmock environment
###########################################################
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

###########################################################
# Setup Code Coverage (Linux only)
###########################################################
if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "-O0 -coverage")

  find_program(LCOV lcov REQUIRED)
  find_program(GENHTML genhtml REQUIRED)

  add_custom_target(coverage
    COMMAND ${LCOV} --directory . --capture --output-file coverage.info
    COMMAND ${LCOV} --remove coverage.info '/usr*' '*gtest*' '*gmock*' --output-file coverage.info
    COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

###########################################################
# Setup Doxygen documentation
###########################################################
find_package(Doxygen)
option(BUILD_DOC "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if (BUILD_DOC)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen need to be installed to generate the doxygen documentation.")
  endif()

  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

  add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
endif (BUILD_DOC)

###########################################################
# Add the static library target MidiDmxBridge
###########################################################
add_library(mididmxbridge STATIC
  MidiDmxBridge/src/midi_dmx/ContinuousController.cpp
  MidiDmxBridge/src/midi_dmx/Dmx.cpp
  MidiDmxBridge/src/midi_dmx/DmxValue.cpp
  MidiDmxBridge/src/midi_dmx/MidiDmxBridge.cpp
  MidiDmxBridge/src/midi_dmx/MidiReader.cpp)

set_target_properties(mididmxbridge PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR})

target_include_directories(mididmxbridge PUBLIC
  MidiDmxBridge/src
  MidiDmxBridge/src/midi_dmx)

target_compile_options(mididmxbridge PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)

###########################################################
# Add binary target for the unit test executable
###########################################################
add_executable(unittests
  tests/MidiDmxBridge/ContinuousControllerTests.cpp
  tests/MidiDmxBridge/DmxTests.cpp
  tests/MidiDmxBridge/DmxValueTests.cpp
  tests/MidiDmxBridge/MidiDmxBridgeTests.cpp
  tests/MidiDmxBridge/MidiReaderTests.cpp
  tests/MidiDmxBridge/UtilTests.cpp
  tests/MidiDmxBridge/VectorTests.cpp
  tests/mocks/SerialReaderMock.cpp)
target_include_directories(unittests PRIVATE tests/mocks)
target_link_libraries(unittests gtest gmock GTest::gtest_main)
target_link_libraries(unittests mididmxbridge)

include(GoogleTest)
gtest_discover_tests(unittests)

###########################################################
# activate cpack
###########################################################
include(CPack)
