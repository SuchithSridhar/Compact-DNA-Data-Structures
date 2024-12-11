include(FetchContent)

cmake_minimum_required(VERSION 3.0)

project(file-utils)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c18")

add_library(file-utils STATIC file_utils.c)

target_include_directories(file-utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
