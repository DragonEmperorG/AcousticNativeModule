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

### INCLUDE OBOE LIBRARY ###

# Set the path to the Oboe library directory
set (OBOE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../oboe)

# Add the Oboe library as a subproject. Since Oboe is an out-of-tree source library we must also
# specify a binary directory
add_subdirectory(${OBOE_DIR} ./oboe-bin)

# Specify the path to the Oboe header files.
include_directories(${OBOE_DIR}/include)

### END OBOE INCLUDE SECTION ###

# Debug utilities
set (DEBUG_UTILS_PATH "./src/main/cpp/debug-utils")
set (DEBUG_UTILS_SOURCES ${DEBUG_UTILS_PATH}/trace.cpp)
include_directories(${DEBUG_UTILS_PATH})

# App specific sources
set (ACOUSTICLOCSDK_DIR src/main/cpp)
file (GLOB_RECURSE ACOUSTICLOCSDK_SOURCES
    ${ACOUSTICLOCSDK_DIR}/jni_bridge.cpp
    ${ACOUSTICLOCSDK_DIR}/AcousticsEngine.cpp
)

# Build the libacoustic-loc-sdk library
add_library(acousticlocsdk
    SHARED
        ${DEBUG_UTILS_SOURCES}
        ${ACOUSTICLOCSDK_SOURCES})

# Specify the libraries needed for acoustic-loc-sdk
target_link_libraries(acousticlocsdk
    PRIVATE
        oboe
        android
        atomic
        log)

# Enable optimization flags: if having problems with source level debugging,
# disable -Ofast ( and debug ), re-enable after done debugging.
target_compile_options(acousticlocsdk PRIVATE -Wall -Werror "$<$<CONFIG:RELEASE>:-Ofast>")
