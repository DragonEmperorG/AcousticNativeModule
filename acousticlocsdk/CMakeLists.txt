#
# Copyright 2017 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

### INCLUDE OBOE LIBRARY ###

# Set the path to the Oboe library directory
set (OBOE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../oboe)

# Add the Oboe library as a subproject. Since Oboe is an out-of-tree source library we must also
# specify a binary directory
add_subdirectory(${OBOE_DIR} ./oboe-bin)

# Specify the path to the Oboe header files.
include_directories(${OBOE_DIR}/include)

### END OBOE INCLUDE SECTION ###


### INCLUDE LIBSNDFILE LIBRARY ###

# Set the path to the LIBSNDFILE library directory
set (LIB_SND_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../libsndfile)

# Add the LIBSNDFILE library as a subproject. Since Oboe is an out-of-tree source library we must also
# specify a binary directory
add_subdirectory(${LIB_SND_FILE_DIR} ./sndfile-bin)

# Specify the path to the LIBSNDFILE header files.
include_directories(${LIB_SND_FILE_DIR}/src)

### END LIBSNDFILE INCLUDE SECTION ###


# Debug utilities
set (UTILS_PATH "src/main/cpp/utils")
set (UTILS_SOURCES ${UTILS_PATH}/trace.cpp)
include_directories(${UTILS_PATH})

# App specific sources
set (ACOUSTICLOCSDK_DIR src/main/cpp)
file (GLOB_RECURSE ACOUSTICLOCSDK_SOURCES
    ${ACOUSTICLOCSDK_DIR}/jni_bridge.cpp
    ${ACOUSTICLOCSDK_DIR}/AcousticsEngine.cpp
    ${ACOUSTICLOCSDK_DIR}/recorder/SoundRecording.cpp
    ${ACOUSTICLOCSDK_DIR}/recorder/RecordingCallback.cpp
    ${ACOUSTICLOCSDK_DIR}/player/PlayingCallback.cpp
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# Build the libacoustic-loc-sdk library
add_library( # Sets the name of the library.
        acousticlocsdk

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${DEBUG_UTILS_SOURCES}
        ${ACOUSTICLOCSDK_SOURCES})

# Specify the libraries needed for acoustic-loc-sdk
target_link_libraries(acousticlocsdk
        ${log-lib}
        oboe
        sndfile)

# Enable optimization flags: if having problems with source level debugging,
# disable -Ofast ( and debug ), re-enable after done debugging.
target_compile_options(acousticlocsdk PRIVATE -Wall -Werror "$<$<CONFIG:RELEASE>:-Ofast>")
