cmake_minimum_required(VERSION 3.25.1)
project(Sparsegray)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sparsegray INTERFACE)
target_include_directories(sparsegray INTERFACE include)

add_subdirectory(tests)

add_subdirectory(dynamic_bitset)
find_package(Boost REQUIRED)
target_link_libraries(sparsegray INTERFACE Boost::boost)