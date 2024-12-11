include(FetchContent)

cmake_minimum_required(VERSION 3.0)

project(bwt-build)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c18")

add_executable(bwt-build bwt_generator.c bwt.c)

target_link_libraries(bwt-build PUBLIC file-utils)

target_compile_options(bwt-build PUBLIC -O2)
