cmake_minimum_required(VERSION 3.15)

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
else()
    message(FATAL_ERROR "Unsupported platform. Only MacOS and Linux are supported.")
endif()

# Create imported static library target for liblo
add_library(liblo_juce STATIC IMPORTED)
set_target_properties(liblo_juce PROPERTIES
    IMPORTED_LOCATION "${LO_LIBRARY_PATH}"
    INTERFACE_INCLUDE_DIRECTORIES "${LO_INCLUDE_DIR}"
)

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

# Link liblo
target_link_libraries(OSCControlApp PRIVATE liblo_juce)

# Compile definitions
target_compile_definitions(OSCControlApp
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:OSCControlApp,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:OSCControlApp,JUCE_VERSION>")
