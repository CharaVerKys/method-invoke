cmake_minimum_required(VERSION 3.5)
project(testLib LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# set(CMAKE_CXX_COMPILER "clang++-15")

include_directories(
            ${CMAKE_SOURCE_DIR}/tests/testing
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/asio/include
            )

add_subdirectory(tests/future)