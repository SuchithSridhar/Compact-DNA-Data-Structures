include(FetchContent)

cmake_minimum_required(VERSION 3.0)

project(ssvector)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c18")

add_library(ssvector STATIC ssvector.c)

target_include_directories(ssvector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
