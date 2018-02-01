cmake_minimum_required(VERSION 3.9)
project(vc)

include(CMakeToolsHelpers OPTIONAL)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_subdirectory(lib)
add_subdirectory(tests)

