cmake_minimum_required(VERSION 3.22)
project(SortedSparseMatrix)

set(CMAKE_CXX_STANDARD 20)

add_library(SortedSparseMatrix
        src/SortedSparseMatrix.h)

set_target_properties(SortedSparseMatrix PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(SortedSparseMatrix PUBLIC src)
