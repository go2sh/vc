cmake_minimum_required(VERSION 3.10)
project(vc)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake" ${PROJECT_BINARY_DIR})

include(CompilerWarnings)
include(conan)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

conan_cmake_run(
    CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

find_package(fmt REQUIRED)
find_package(Catch2)

add_subdirectory(lib)
add_subdirectory(tests)

set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER} CACHE FILEPATH "Compiler")
set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER} CACHE FILEPATH "Compiler")

