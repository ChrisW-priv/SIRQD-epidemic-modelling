cmake_minimum_required(VERSION 3.22)
project(SIRQD_epidemic_modelling)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp SIRQD_bulk.cpp)

add_subdirectory(SortedSparseMatrix)
target_link_libraries(${PROJECT_NAME} PUBLIC SortedSparseMatrix)
