cmake_minimum_required(VERSION 3.11)
project(allocation_test)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")

include(FindThreads)

if(NOT WIN32)
    find_package(PkgConfig QUIET)
endif()

if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBPMEMOBJ++ REQUIRED libpmemobj++)
else()
    find_package(LIBPMEMOBJ++ REQUIRED)
endif()

link_directories(${LIBPMEMOBJ++_LIBRARY_DIRS})

add_executable(allocation_test main.cpp)

target_include_directories(allocation_test PUBLIC ${LIBPMEMOBJ++_INCLUDE_DIRS})

target_link_libraries(allocation_test ${LIBPMEMOBJ++_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})