cmake_minimum_required(VERSION 3.25.1)
project(VisionEngine)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_TOOLCHAIN_FILE "/Users/user/Construction/vcpkg/scripts/buildsystems/vcpkg.cmake")

add_library(sparsegray INTERFACE)
target_include_directories(sparsegray INTERFACE include)

add_subdirectory(tests)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(sparsegray INTERFACE Boost::boost)