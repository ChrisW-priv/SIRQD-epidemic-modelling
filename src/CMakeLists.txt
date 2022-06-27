# link with library
add_subdirectory(SortedSparseMatrix)
target_compile_options(SortedSparseMatrix PUBLIC "-O3")
target_link_libraries(${PROJECT_NAME} PUBLIC SortedSparseMatrix)
