cmake_minimum_required(VERSION 3.27)
project(aoc-2023)

include(FetchContent)
FetchContent_Declare(lexy URL https://lexy.foonathan.net/download/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_BUILD_TYPE Debug)

find_package(doctest REQUIRED)

add_executable(01-p1 ./01-p1/main.cpp)
add_executable(01-p2 ./01-p2/main.cpp)
add_executable(02-p1 ./02-p1/main.cpp)
add_executable(02-p2 ./02-p2/main.cpp)
add_executable(03-p1 ./03-p1/main.cpp)
add_executable(03-p2 ./03-p2/main.cpp)

add_executable(04 04/main.cpp 04/lib.cpp 04/lib.hpp)
add_executable(04-tests 04/tests.cpp 04/lib.cpp 04/lib.hpp)

add_executable(05 05/lib.cpp 05/lib.hpp 05/main.cpp)
add_executable(05-tests 05/lib.cpp 05/lib.hpp 05/tests.cpp)
add_executable(05-p2 05-p2/lib.cpp 05-p2/lib.hpp 05-p2/main.cpp)
add_executable(05-p2-tests 05-p2/lib.cpp 05-p2/lib.hpp 05-p2/tests.cpp)

target_link_libraries(02-p1 PRIVATE foonathan::lexy)
target_link_libraries(02-p2 PRIVATE foonathan::lexy)

target_link_libraries(04 PRIVATE foonathan::lexy)
target_link_libraries(04-tests PRIVATE foonathan::lexy)
target_link_libraries(04-tests PRIVATE doctest::doctest)

target_link_libraries(05 PRIVATE foonathan::lexy)
target_link_libraries(05-tests PRIVATE foonathan::lexy)
target_link_libraries(05-tests PRIVATE doctest::doctest)
