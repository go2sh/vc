enable_testing()

include(Catch)

add_library(Catch2_main Catch2Main.cpp)
target_include_directories(Catch2_main PUBLIC ${Catch2_INCLUDE_DIRS})

add_executable(SupportTests 
    #unit/Support/StringMapTest.cpp
    unit/Support/MemoryFileSystemTest.cpp
    #unit/Support/FunctionRefTest.cpp
)
target_link_libraries(SupportTests vcSupport Catch2::Catch2)
catch_discover_tests(SupportTests)