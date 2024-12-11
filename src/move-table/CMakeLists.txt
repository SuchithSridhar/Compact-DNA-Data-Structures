include(FetchContent)

cmake_minimum_required(VERSION 3.0)

project(move-table)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c18")

add_executable(move-mem-finder move_mem_finder.c)
target_compile_options(move-mem-finder PUBLIC -O2)
target_link_libraries(move-mem-finder PUBLIC string-utils)

add_executable(move-build-forward make_table.c)
target_compile_options(move-build-forward PUBLIC -O2)

add_executable(move-build-reverse make_table_rev.c)
target_compile_options(move-build-reverse PUBLIC -O2)
