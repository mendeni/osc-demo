cmake_minimum_required(VERSION 3.15)

# Set macOS architectures before project() to build universal binaries
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
endif()

project(OSCControlApp VERSION 1.0.0)

# Add JUCE
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Platform detection and liblo configuration
if(APPLE)
    set(LO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/mac/include")
    set(LO_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/deps/mac/lib/liblo.a")
elseif(UNIX AND NOT APPLE)
    set(LO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/linux/include")
    set(LO_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/deps/linux/lib/liblo.a")
elseif(WIN32)
    set(LO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/windows/include")
    set(LO_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/deps/windows/lib/liblo.lib")
else()
    message(FATAL_ERROR "Unsupported platform. Only MacOS and Linux are supported.")
endif()

# Define the application
juce_add_gui_app(OSCControlApp
    PRODUCT_NAME "OSC Control Demo")

# Add source files
target_sources(OSCControlApp
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/MainComponent.h)

# Link JUCE modules
target_link_libraries(OSCControlApp
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Link liblo - include directories and link static library directly with absolute path
target_include_directories(OSCControlApp PRIVATE ${LO_INCLUDE_DIR})
target_link_libraries(OSCControlApp PRIVATE "${LO_LIBRARY_PATH}")
# Link winsock on Windows
elseif(WIN32)
    target_link_libraries(osc_host PRIVATE ws2_32)
endif()

# Compile definitions
target_compile_definitions(OSCControlApp
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:OSCControlApp,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:OSCControlApp,JUCE_VERSION>")
