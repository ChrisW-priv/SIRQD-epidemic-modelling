cmake_minimum_required(VERSION 3.22)
project(double_buffer)

set(CMAKE_CXX_STANDARD 20)

add_library(double_buffer
        src/double_buffer.h)

set_target_properties(double_buffer PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(double_buffer PUBLIC src)
