#!/bin/sh -x

if [ ! -e JUCE/CMakeLists.txt ]; then
  git submodule update --init --recursive
fi

rm -rf build

mkdir build && cd build

cmake .. && make
