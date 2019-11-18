cmake_minimum_required (VERSION 2.8)

project (mem_allocator)

aux_source_directory(. DIR_SRCS)

add_executable(mem_test ${DIR_SRCS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -std=gnu99")