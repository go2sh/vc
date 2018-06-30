enable_testing()

include(Catch)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

#add_executable(SupportTests unit/Support/StringMapTest.cpp "${CATCH_INCLUDE_DIR}/catch.hpp")
#target_link_libraries(SupportTests vc Catch)
#catch_discover_tests(SupportTests)
add_executable(format_test format_test.cpp)
target_link_libraries(format_test vc)

add_executable(stdio_test stdio_test.cpp)
target_link_libraries(stdio_test vc)

add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test vc)