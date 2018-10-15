enable_testing()

include(Catch)

add_library(Catch2_main Catch2Main.cpp)
target_include_directories(Catch2_main PUBLIC ${Catch2_INCLUDE_DIRS})

add_executable(SupportTests 
    unit/Support/StringMapTest.cpp
    unit/Support/MemoryFileSystemTest.cpp
)
target_link_libraries(SupportTests vcSupport vc Catch2_main)
catch_discover_tests(SupportTests)

add_executable(format_test format_test.cpp)
target_link_libraries(format_test vc vcSupport vcFormat ${Boost_LIBRARIES})

add_executable(stdio_test stdio_test.cpp)
target_link_libraries(stdio_test vc)

add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test vc)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(test test.cpp)
target_link_libraries(test vc ${Boost_LIBRARIES})