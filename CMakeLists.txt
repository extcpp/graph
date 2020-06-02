# Copyright - 2016-2020 - Jan Christoph Uhde <Jan@UhdeJC.com>

cmake_minimum_required(VERSION 3.16)
project(ext-graph VERSION 0.0.1 LANGUAGES CXX)
message(STATUS "extINFO -- entering ext-graph")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## OPTIONS
option(EXTGRAPH_EXAMPLES "build examples" OFF)
option(EXTGRAPH_WARNINGS "enable warnings" ON)
option(EXTGRAPH_CHECKED "user assert" ON)

option(EXTGRAPH_TESTS "build tests" OFF)

## general setup and includes
set(CMAKE_CXX_STANDARD 20 CACHE STRING "doc")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "")
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # XCode / VS folders

find_package(Threads REQUIRED)

set(LIBEXT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/ext-basics)
if(NOT TARGET ext::basics)
    add_subdirectory(${LIBEXT_SOURCE_DIR} REQUIRED)
    list(APPEND CMAKE_MODULE_PATH "${LIBEXT_SOURCE_DIR}/cmake")
endif()

include(ext_cmake_setup)
include(ext_cmake_install)

set(FORCE_COLORED_OUTPUT ON)
ext_colored_compiler_ouput()

ext_log("standard ${CMAKE_CXX_STANDARD}")

# verbose windows linking
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /VERBOSE:LIB")

if(LINUX)
    set(EXT_OUTDIR "")
elseif(MSVC)
    #TODO - move settings below into corresponding targets
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${EXT_OUTDIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${EXT_OUTDIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXT_OUTDIR}")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

# include build dir to find version.hpp
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# required by folder structure for XCode and VisualStudio (includes)
# sources are always required
include(src_files.cmake)
include(include_files.cmake)

add_library(ext-graph INTERFACE ${ext-graph-source} ${ext-graph-header})
target_include_directories(ext-graph INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

target_compile_features(ext-graph INTERFACE cxx_std_20)
target_compile_definitions(ext-graph INTERFACE EXTGRAPH_HEADER_ONLY_LIB)
target_compile_definitions(ext-graph INTERFACE $<$<BOOL:${EXTGRAPH_EXCEPTIONS}>:EXTGRAPH_EXCEPTIONS=1>)

# set up folder structure for XCode and VisualStudio
# set_target_properties (ext-graph PROPERTIES FOLDER libext) # not for INTERFACE libs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${libext_header} ${libext_source})

add_library(ext::graph ALIAS ext-graph)

## testing
if(EXTGRAPH_TESTS)
    ext_log("ext-graph tests enabled")
    include(CTest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    ext_add_test_subdirectory("google" tests)
else()
    ext_log("ext-graph tests disabled")
endif()

## ## add projects using this lib
## if(EXTGRAPH_EXAMPLES)
##     ext_log("ext-graph examples enabled")
##     add_subdirectory(examples)
## else()
##     ext_log("ext-graph examples disabled")
## endif()

## installation
if(COMMAND ext_install)
    set_target_properties(ext-graph PROPERTIES EXPORT_NAME graph)

    ext_install(ext-graph include/ext)

    install(TARGETS ext-graph
        EXPORT ext-graph-targets
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
	install(
        EXPORT ext-graph-targets
		FILE ext-graph-config.cmake
        NAMESPACE ext::
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
endif()

add_custom_target(
    update_graph_version ALL
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND}
                -D "EXT_GIT_VERSION_OUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/ext_version.hpp"
                -P "${LIBEXT_SOURCE_DIR}/cmake/ext_script_git_version.cmake"
)
