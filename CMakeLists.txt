cmake_minimum_required(VERSION 3.11)
project(PMDK_allocation_test)

set(CMAKE_CXX_STANDARD 17)

find_library(pmem libpmemobj++ REQUIRED)

add_executable(PMDK_allocation_test main.cpp)

target_link_libraries(PMDK_allocation_test pmem)