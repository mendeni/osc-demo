cmake_minimum_required(VERSION 3.10)
project(osc_demo VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find liblo package
find_library(LO_LIBRARY lo REQUIRED)
find_path(LO_INCLUDE_DIR lo/lo.h REQUIRED)

# Include directories
include_directories(${LO_INCLUDE_DIR})

# Add executable
add_executable(osc_host src/main.cpp)

# Link libraries
target_link_libraries(osc_host ${LO_LIBRARY})

# Installation
install(TARGETS osc_host DESTINATION bin)
