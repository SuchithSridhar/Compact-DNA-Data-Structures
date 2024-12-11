include(FetchContent)

cmake_minimum_required(VERSION 3.0)

project(rlfm-index)

include_directories("include")

#----------------------------------------
# Add sdsl
#----------------------------------------
FetchContent_Declare(
  sdsl
  GIT_REPOSITORY https://github.com/simongog/sdsl-lite
)

FetchContent_GetProperties(sdsl)
if(NOT sdsl_POPULATED)
  FetchContent_Populate(sdsl)

  set(GENERATE_DOC OFF CACHE BOOL "Do not generate doxygen for sdsl-lite")

  add_subdirectory(${sdsl_SOURCE_DIR} ${sdsl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
# compile with address sanitizer
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

add_executable(rlfm-build-index build.cpp)
target_link_libraries(rlfm-build-index sdsl)
target_compile_options(rlfm-build-index PUBLIC -O2)

add_executable(rlfm-load-index load.cpp)
target_link_libraries(rlfm-load-index sdsl)
target_compile_options(rlfm-load-index PUBLIC -O2)

add_executable(rlfm-invert-index invert.cpp rlfm.cpp rlfm.h)
target_link_libraries(rlfm-invert-index sdsl)
target_compile_options(rlfm-invert-index PUBLIC -O2)

add_executable(rlfm-movelet-invert invert_movelet.cpp rlfm.cpp rlfm.h)
target_link_libraries(rlfm-movelet-invert sdsl)
target_compile_options(rlfm-movelet-invert PUBLIC -O2)

add_executable(rlfm-mem-finder mem_finder.cpp rlfm.cpp rlfm.h)
target_link_libraries(rlfm-mem-finder sdsl)
target_compile_options(rlfm-mem-finder PUBLIC -O2)
