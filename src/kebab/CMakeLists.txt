include(FetchContent)

cmake_minimum_required(VERSION 3.0)

project(kebab)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c18")

add_executable(kebab-build build_kmer_filter.c bitvector.c bloom_filter.c kmer_filter.c)
target_link_libraries(kebab-build PUBLIC m) # link the math library
target_link_libraries(kebab-build PUBLIC file-utils)

add_executable(kebab-filter kmer_substrings.c bitvector.c bloom_filter.c kmer_filter.c)
target_compile_options(kebab-filter PUBLIC -O2)
target_link_libraries(kebab-filter PUBLIC string-utils)
target_link_libraries(kebab-filter PUBLIC file-utils)

