cmake_minimum_required(VERSION 3.10)
project(vc)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

include(CMakeToolsHelpers OPTIONAL)
include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)


include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_subdirectory(lib)
add_subdirectory(tests)

set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER} CACHE FILEPATH "Compiler")
set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER} CACHE FILEPATH "Compiler")

