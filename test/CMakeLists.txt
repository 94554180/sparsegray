include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/dynamic_bitset)

add_executable(sparsegray_test sparsegray_test.cpp)
target_link_libraries(sparsegray_test sparsegray)

add_test(NAME sparsegray_test COMMAND sparsegray_test)