cmake_minimum_required(VERSION 3.27)
project(aoc-2023)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_BUILD_TYPE Debug)

add_executable(01-p1 ./01-p1/main.cpp)
add_executable(01-p2 ./01-p2/main.cpp)
