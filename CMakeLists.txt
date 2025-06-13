cmake_minimum_required(VERSION 4.0.3)

project(gr-threadpool VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(gr-threadpool main.cpp threadpool.cpp)

