cmake_minimum_required(VERSION 3.0)
project(GenomeIndexing)

# Define options for building components
option(ALL "Build all components" OFF)
option(RLFM "Build RLFM Related Code" OFF)
option(FM "Build FM Index Related Code" OFF)
option(BWT "Build BWT Related Code" OFF)
option(MOVE "Build Move Table Related Code" OFF)
option(KEBAB "Build KEBAB Related Code" OFF)

# Define output directory structure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # For executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # For shared libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # For static libraries

# Helper function to check if a specific option is set
macro(option_is_set option_name var)
    if(${option_name})
        set(${var} TRUE)
    else()
        set(${var} FALSE)
    endif()
endmacro()

# Detect if no flags are provided
set(USER_PROVIDED_ANY_OPTION OFF)
foreach(opt ALL RLFM FM BWT MOVE KEBAB)
    option_is_set(${opt} IS_SET)
    if(IS_SET)
        set(USER_PROVIDED_ANY_OPTION ON)
        break()
    endif()
endforeach()

if(NOT USER_PROVIDED_ANY_OPTION)
    message(FATAL_ERROR "No build option specified. Please provide one or more options:
    -DALL=ON
    -DRLFM=ON
    -DFM=ON
    -DBWT=ON
    -DMOVE=ON
    -DKEBAB=ON
    ")
endif()

# If ALL is ON, enable all components
if(ALL)
    set(RLFM ON)
    set(FM ON)
    set(BWT ON)
    set(MOVE ON)
    set(KEBAB ON)
endif()

include_directories(src/include)

add_subdirectory(src/file-utils)
add_subdirectory(src/string-utils)

if(RLFM)
    add_subdirectory(src/rlfm)
endif()

if(FM)
    add_subdirectory(src/fm-index)
endif()

if(MOVE)
    add_subdirectory(src/move-table)
endif()

if(KEBAB)
    add_subdirectory(src/kebab)
endif()

if(BWT)
    add_subdirectory(src/bwt)
endif()

