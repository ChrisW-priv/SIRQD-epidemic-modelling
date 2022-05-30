cmake_minimum_required(VERSION 3.22)
project(SIRQD_epidemic_modelling)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp SIRQD_bulk.cpp SIRQD_bulk.h)
target_compile_options(${PROJECT_NAME} PUBLIC "-O3")

add_subdirectory(SortedSparseMatrix)
target_compile_options(SortedSparseMatrix PUBLIC "-O3")
target_link_libraries(${PROJECT_NAME} PUBLIC SortedSparseMatrix)
