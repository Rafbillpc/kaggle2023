cmake_minimum_required(VERSION 3.22)
message(STATUS "CMake version: ${CMAKE_VERSION}")
project(icfpc2023
  LANGUAGES CXX)

# Options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
  -Wall -Wextra
  -Wno-unused-parameter -Wno-unused-function -Wno-unused-const-variable
  )
add_compile_options(-march=native)

# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
add_link_options("-fuse-ld=mold")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  add_link_options(-fsanitize=address)
  add_compile_options(-fsanitize=address)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

# CCache

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Dependencies

find_package(OpenMP REQUIRED)

# add_subdirectory(external/indicators)
add_subdirectory(external/argparse)
add_subdirectory(external/json)
add_subdirectory(external/backward-cpp)

# Common
add_library(common
  src/header.cpp
  src/puzzle_info.cpp
  src/util.cpp
  src/rubik.cpp
  src/rubik_init.cpp
  src/rubik_transitions.cpp
  src/rubik_print.cpp
  src/rubik_solve.cpp
  src/rubik_222.cpp
  src/optimal_333.cpp
  src/kociemba_phase1.cpp
  src/kociemba_phase2.cpp
  src/rubik_444_phase1.cpp
  src/rubik_444_phase2.cpp
  src/rubik_444_phase3.cpp
  src/rubik_555_phase1.cpp
  src/rubik_555_phase2.cpp
  src/rubik_555_phase3.cpp
  src/rubik_555_phase4.cpp
  src/rubik_555_phase5.cpp
  src/rubik_cycles.cpp
  )
target_include_directories(common PUBLIC
  src)
target_link_libraries(common PUBLIC
  OpenMP::OpenMP_CXX
  nlohmann_json::nlohmann_json
  Backward::Backward
  argparse::argparse
  )
target_precompile_headers(common PRIVATE src/header.hpp)

# Main
add_executable(main
  src/main.cpp)
target_link_libraries(main PUBLIC common)
target_precompile_headers(main REUSE_FROM common)

# Stats
add_executable(stats
  src/stats.cpp)
target_link_libraries(stats PUBLIC common)
target_precompile_headers(stats REUSE_FROM common)

# Merge
add_executable(merge
  src/merge.cpp)
target_link_libraries(merge PUBLIC common)
target_precompile_headers(merge REUSE_FROM common)

