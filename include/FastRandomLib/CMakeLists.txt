cmake_minimum_required(VERSION 3.22)
project(FastRandomLib)

set(CMAKE_CXX_STANDARD 20)

add_library(FastRandomLib
        src/fast_random.cpp
        src/fast_random.h
        )

set_target_properties(FastRandomLib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(FastRandomLib PUBLIC src)
