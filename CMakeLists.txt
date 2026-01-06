cmake_minimum_required(VERSION 3.15)
project(osc_demo VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# macOS universal binary support - build for both x86_64 and arm64
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
endif()

# Find liblo package
find_library(LO_LIBRARY lo REQUIRED)
find_path(LO_INCLUDE_DIR lo/lo.h REQUIRED)

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
