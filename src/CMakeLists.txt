# link with library
add_subdirectory(FastRandomLib)
target_link_libraries(${PROJECT_NAME} PUBLIC FastRandomLib)

add_subdirectory(SortedSparseMatrix)
target_link_libraries(${PROJECT_NAME} PUBLIC SortedSparseMatrix)
