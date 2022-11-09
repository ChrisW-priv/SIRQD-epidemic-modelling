cmake_minimum_required(VERSION 3.22)
project(SIRQD_epidemic_modelling)

set(CMAKE_CXX_STANDARD 20)

#create main executable
add_executable(${PROJECT_NAME} src/main.cpp src/SIRQD_bulk.cpp src/SIRQD_bulk.h)
target_compile_options(${PROJECT_NAME} PUBLIC "-O3")

add_subdirectory(include)
