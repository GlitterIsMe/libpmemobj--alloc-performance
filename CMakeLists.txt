cmake_minimum_required(VERSION 3.11)
project(PMDK_allocation_test)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")

find_library(pmem libpmemobj++ REQUIRED)

add_executable(PMDK_allocation_test main.cpp)

target_link_libraries(PMDK_allocation_test pmem)