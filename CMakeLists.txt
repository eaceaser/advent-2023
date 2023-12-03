cmake_minimum_required(VERSION 3.27)
project(aoc-2023)

include(FetchContent)
FetchContent_Declare(lexy URL https://lexy.foonathan.net/download/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_BUILD_TYPE Debug)

add_executable(01-p1 ./01-p1/main.cpp)
add_executable(01-p2 ./01-p2/main.cpp)
add_executable(02-p1 ./02-p1/main.cpp)
add_executable(02-p2 ./02-p2/main.cpp)
add_executable(03-p1 ./03-p1/main.cpp)

target_link_libraries(02-p1 PRIVATE foonathan::lexy)
target_link_libraries(02-p2 PRIVATE foonathan::lexy)
