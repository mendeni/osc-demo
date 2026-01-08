cmake_minimum_required(VERSION 3.15)
project(osc_demo VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Platform detection and liblo configuration
if(APPLE)
    set(LO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/mac/include")
    set(LO_LIBRARY "${CMAKE_SOURCE_DIR}/deps/mac/lib/liblo.a")
elseif(UNIX AND NOT APPLE)
    set(LO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/linux/include")
    set(LO_LIBRARY "${CMAKE_SOURCE_DIR}/deps/linux/lib/liblo.a")
else()
    message(FATAL_ERROR "Unsupported platform. Only MacOS and Linux are supported.")
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
