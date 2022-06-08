cmake_minimum_required(VERSION 3.19)
project(min)

enable_testing()
set(CMAKE_CXX_STANDARD 20)

include_directories(src/main)

# min lib
file(GLOB_RECURSE MIN_LIB_SOURCE_FILES "src/main/min/vm/*.cc" "src/main/min/common/*.cc")
add_library(min ${MIN_LIB_SOURCE_FILES})

# test
add_executable(test_result "src/main/min/test_result.cc")
target_link_libraries(test_result min)

add_executable(test_vm "src/main/min/test_vm.cc")
target_link_libraries(test_vm min)