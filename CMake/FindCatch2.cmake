include(LibFindMacros)

libfind_pkg_check_modules(Catch2_PKGCONF catch2)

find_path(Catch2_INCLUDE_DIR
  NAMES catch2/catch.hpp
  PATHS ${Catch2_PKGCONF_INCLUDE_DIRS}
)

if(NOT EXISTS "${Catch2_INCLUDE_DIR}")
    ExternalProject_Add(Catch2
        URL https://github.com/catchorg/Catch2/releases/download/v2.2.3/catch.hpp
        DOWNLOAD_NO_EXTRACT ON
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory "<INSTALL_DIR>/include/catch2"
        COMMAND ${CMAKE_COMMAND} -E copy "<DOWNLOAD_DIR>/catch.hpp" "<INSTALL_DIR>/include/catch2"
    )

    ExternalProject_Get_Property(Catch2 install_dir)
    set (Catch2_INCLUDE_DIR ${install_dir}/include)
endif()

set(Catch2_PROCESS_INCLUDES Catch2_INCLUDE_DIR)
libfind_process(Catch2)
