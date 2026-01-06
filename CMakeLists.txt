cmake_minimum_required(VERSION 3.15)
project(osc_demo VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# macOS universal binary support - build for both x86_64 and arm64
# Note: Requires liblo and other dependencies to be available as universal binaries
option(OSC_DEMO_UNIVERSAL_BINARY "Build macOS universal binary for x86_64 and arm64" OFF)

if(APPLE AND OSC_DEMO_UNIVERSAL_BINARY)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
    message(STATUS "Building macOS universal binary (x86_64 and arm64)")
endif()

# Find liblo package
# Try to find static library first for better portability
find_library(LO_LIBRARY 
    NAMES liblo.a lo
    REQUIRED)
find_path(LO_INCLUDE_DIR lo/lo.h REQUIRED)

# Check if we found a static library
get_filename_component(LO_LIBRARY_EXT ${LO_LIBRARY} EXT)
if(LO_LIBRARY_EXT STREQUAL ".a")
    message(STATUS "Found static liblo library: ${LO_LIBRARY}")
else()
    message(STATUS "Found dynamic liblo library: ${LO_LIBRARY}")
    message(WARNING "Static liblo library not found. Binary will require liblo to be installed at runtime.")
endif()

# Include directories
include_directories(${LO_INCLUDE_DIR})

# Add executable
add_executable(osc_host src/main.cpp)

# Link libraries
target_link_libraries(osc_host ${LO_LIBRARY})

# Add JUCE application subdirectory
add_subdirectory(juce_osc_app)

# Installation
install(TARGETS osc_host DESTINATION bin)
