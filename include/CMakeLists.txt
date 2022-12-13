project(include)

add_library(include SHARED FastRandomLib SortedSparseMatrix double_buffer)
add_subdirectory(FastRandomLib)
add_subdirectory(SortedSparseMatrix)
add_subdirectory(double_buffer)
target_link_libraries(${PROJECT_NAME} PUBLIC FastRandomLib)
target_link_libraries(${PROJECT_NAME} PUBLIC SortedSparseMatrix)
target_link_libraries(${PROJECT_NAME} PUBLIC double_buffer)

set_target_properties(include PROPERTIES POSITION_INDEPENDENT_CODE True)
set_target_properties(include PROPERTIES LINKER_LANGUAGE CXX)
