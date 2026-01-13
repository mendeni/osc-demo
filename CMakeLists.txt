cmake_minimum_required(VERSION 3.15)

# Set macOS architectures before project() to build universal binaries
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
endif()

project(osc_demo VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Platform detection and liblo configuration
if(APPLE)
    set(LO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/mac/include")
    set(LO_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/deps/mac/lib/liblo.a")
elseif(UNIX AND NOT APPLE)
    set(LO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/linux/include")
    set(LO_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/deps/linux/lib/liblo.a")
elseif(WINDOWS)
    set(LO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/windows/include")
    set(LO_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/deps/windows/lib/liblo.a")
else()
    message(FATAL_ERROR "Unsupported platform. Only MacOS, Linux and Windows are supported.")
endif()

# Add executable
add_executable(osc_host src/main.cpp)

# Include directories
target_include_directories(osc_host PRIVATE ${LO_INCLUDE_DIR})

# Link static library directly with absolute path to avoid dynamic library search
target_link_libraries(osc_host PRIVATE "${LO_LIBRARY_PATH}")

# Add JUCE application subdirectory
add_subdirectory(juce_osc_app)

# Installation
install(TARGETS osc_host DESTINATION bin)
