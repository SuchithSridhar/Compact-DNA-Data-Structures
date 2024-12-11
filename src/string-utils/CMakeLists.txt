include(FetchContent)

cmake_minimum_required(VERSION 3.0)

project(string-utils)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c18")

add_library(string-utils STATIC string_utils.c)

target_include_directories(string-utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
