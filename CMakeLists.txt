cmake_minimum_required(VERSION 3.19)
project(min)

enable_testing()
set(CMAKE_CXX_STANDARD 20)

include_directories(src/main)

# min lib
file(GLOB_RECURSE MIN_LIB_SOURCE_FILES "src/main/min/io/*.cc" "src/main/min/vm/*.cc" "src/main/min/common/*.cc")
add_library(min ${MIN_LIB_SOURCE_FILES})

# minas assembler & repl
add_executable(minas "src/main/min/minas.cc")
target_link_libraries(minas min)

# copy files for tests.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/example/min DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/example/)

# test
add_executable(scratch "src/main/min/scratch.cc")

add_executable(test_result "src/main/min/test_result.cc")
target_link_libraries(test_result min)

add_executable(test_vm "src/main/min/test_vm.cc")
target_link_libraries(test_vm min)
