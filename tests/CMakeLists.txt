include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories("/Users/user/Construction/vcpkg/packages/boost-dynamic-bitset_arm64-osx/include/boost")

add_executable(sparsegray_test sparsegray_test.cpp)
target_link_libraries(sparsegray_test sparsegray)

add_test(NAME sparsegray_test COMMAND sparsegray_test)