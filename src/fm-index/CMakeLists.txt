include(FetchContent)

cmake_minimum_required(VERSION 3.0)

project(fm-index)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c18")

add_executable(fm-mem-finder mem_finder.c fm_index.c)
target_compile_options(fm-mem-finder PUBLIC -O2)
