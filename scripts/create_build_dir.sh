#!/bin/sh
cmake -S . -B build/debug -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++-15
cmake -S . -B build/release -G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_COMPILER=clang++-15
