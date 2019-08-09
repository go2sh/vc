find_package(PkgConfig)

pkg_check_modules(fmt_PKGCONF fmt)

find_path(fmt_INCLUDE_DIR
  NAMES fmt.h
  HINTS ${fmt_PKGCONF_INCLUDE_DIRS} 
)

find_library(fmt_LIBRARY
  NAMES libfmt.a
  HINTS ${fmt_PKGCONF_LIBRARY_DIRS}
)

add_library(fmt INTERFACE)

if(NOT fmt_INCLUDE_DIR)
    ExternalProject_Add(fmt-external
        URL https://github.com/fmtlib/fmt/releases/download/5.3.0/fmt-5.3.0.zip
        CMAKE_CACHE_ARGS
          -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>/

    )

    ExternalProject_Get_Property(fmt-external install_dir)
    ExternalProject_Get_Property(fmt-external binary_dir)
    set (fmt_INCLUDE_DIR ${install_dir}/include)
    set (fmt_LIBRARY ${install_dir}/${CMAKE_INSTALL_LIBDIR}/libfmt.a)
    add_dependencies(fmt fmt-external)
endif()

target_include_directories(fmt INTERFACE ${fmt_INCLUDE_DIR})
target_link_libraries(fmt INTERFACE ${fmt_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set fmt_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(fmt  DEFAULT_MSG
                                  fmt_LIBRARY fmt_INCLUDE_DIR)

mark_as_advanced(fmt_INCLUDE_DIR fmt_LIBRARY)

set(fmt_LIBRARIES ${fmt_LIBRARY} )
set(fmt_INCLUDE_DIRS ${fmt_INCLUDE_DIR} )
