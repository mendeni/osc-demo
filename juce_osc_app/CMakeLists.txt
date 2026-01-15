cmake_minimum_required(VERSION 3.15)

# Set macOS architectures before project() to build universal binaries
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
endif()

project(OSCControlApp VERSION 1.0.0)

# JUCE is already added by parent project, so we don't need to add it again

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
        juce::juce_osc
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Compile definitions
target_compile_definitions(OSCControlApp
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:OSCControlApp,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:OSCControlApp,JUCE_VERSION>")
