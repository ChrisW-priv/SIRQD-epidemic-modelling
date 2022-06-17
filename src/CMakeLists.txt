#create main executable
add_executable(${PROJECT_NAME} main.cpp SIRQD_bulk.cpp SIRQD_bulk.h)
target_compile_options(${PROJECT_NAME} PUBLIC "-O3")

# link with library
add_subdirectory(SortedSparseMatrix)
target_compile_options(SortedSparseMatrix PUBLIC "-O3")
target_link_libraries(${PROJECT_NAME} PUBLIC SortedSparseMatrix)
