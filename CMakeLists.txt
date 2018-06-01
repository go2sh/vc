cmake_minimum_required(VERSION 3.10)
project(vc)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

include(CMakeToolsHelpers OPTIONAL)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_DEBUG_LIBS ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIR}
)

link_directories(
    ${Boost_LIBRARY_DIR}
)
add_subdirectory(lib)
add_subdirectory(tests)

