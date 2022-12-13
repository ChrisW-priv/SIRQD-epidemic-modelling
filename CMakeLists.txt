cmake_minimum_required(VERSION 3.22)
project(SIRQD_epidemic_modelling)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_LINKER_LANGUAGE CXX)

#create main executable
add_executable(${PROJECT_NAME} src/main.cpp)
add_library(modeling_library SHARED src/SIRQD_bulk.h src/SIRQD_bulk.cpp)
add_subdirectory(include)
target_link_libraries(modeling_library include)

target_link_libraries(${PROJECT_NAME} PUBLIC modeling_library)
target_compile_options(${PROJECT_NAME} PUBLIC "-O3")
