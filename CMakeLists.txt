cmake_minimum_required(VERSION 3.6)
project(vc)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(lib)
add_subdirectory(tests)

