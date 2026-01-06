cmake_minimum_required(VERSION 3.15)

project(OSCControlApp VERSION 1.0.0)

# Add JUCE
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Find liblo
# Try to find static library first for better portability
find_library(LO_LIBRARY 
    NAMES liblo.a lo
    REQUIRED)
find_path(LO_INCLUDE_DIR lo/lo.h REQUIRED)

# Check if we found a static library
get_filename_component(LO_LIBRARY_EXT ${LO_LIBRARY} EXT)
if(LO_LIBRARY_EXT STREQUAL ".a")
    message(STATUS "Found static liblo library for JUCE app: ${LO_LIBRARY}")
else()
    message(STATUS "Found dynamic liblo library for JUCE app: ${LO_LIBRARY}")
    message(WARNING "Static liblo library not found. JUCE app will require liblo to be installed at runtime.")
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

# Link liblo
target_include_directories(OSCControlApp PRIVATE ${LO_INCLUDE_DIR})
target_link_libraries(OSCControlApp PRIVATE ${LO_LIBRARY})

# Compile definitions
target_compile_definitions(OSCControlApp
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:OSCControlApp,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:OSCControlApp,JUCE_VERSION>")
