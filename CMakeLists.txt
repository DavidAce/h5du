cmake_minimum_required(VERSION 3.15)
project(h5du
        VERSION 0.0.1
        DESCRIPTION "HDF5 Disk Usage"
        HOMEPAGE_URL "")


include(cmake/DeprecationWarnings.cmake) # Warn if user passes deprecated settings

# Make sure we bundled find modules
list(INSERT CMAKE_MODULE_PATH 0  ${PROJECT_SOURCE_DIR}/cmake)

# Set options this build
option(H5DU_INSTALL                     "Enable installation"                                          ON)
option(H5DU_ENABLE_LTO                  "Enable link time optimization"                                OFF)
option(H5DU_ENABLE_TESTS                "Enable unit testing with ctest"                               OFF)
option(H5DU_ENABLE_ASAN                 "Enable runtime address sanitizer -fsanitize=address"          OFF)
option(H5DU_ENABLE_ASAN                 "Enable runtime address sanitizer -fsanitize=address"          OFF)
option(H5DU_BUILD_EXAMPLES              "Build examples"                                               OFF)
option(H5DU_PREFIX_ADD_PKGNAME          "Install dependencies into CMAKE_INSTALL_PREFIX/<libname>"     OFF)
option(CMAKE_POSITION_INDEPENDENT_CODE  "Use -fPIC when compiling shared libraries"                    ON)


# Make an "enum" for valid download methods
set(H5DU_PACKAGE_MANAGERS_VALID find cmake conan)
set(H5DU_PACKAGE_MANAGER find CACHE STRING "Download method for external dependencies")
set_property(CACHE H5DU_PACKAGE_MANAGER PROPERTY STRINGS ${H5DU_PACKAGE_MANAGERS_VALID})
if (NOT H5DU_PACKAGE_MANAGER IN_LIST H5DU_PACKAGE_MANAGERS_VALID)
    message(FATAL_ERROR "H5DU_PACKAGE_MANAGER must be one of ${H5DU_PACKAGE_MANAGERS_VALID}")
endif ()


################################################################
### Get git version number                                   ###
### Generates a header gitversion/gitversion.h               ###
### Include it using #include <gitversion.h>                 ###
### Gives a namespace GIT:: with several git version numbers.###
################################################################
include(cmake/gitversion.cmake)


### Print extra info during CMake configure
include(cmake/PrintBuildInfo.cmake)


###  Add targets to collect common settings
add_library(flags INTERFACE)
add_library(deps INTERFACE)


###  Apply compiler flags
include(cmake/CompilerFlags.cmake)

### Find or install all the dependencies
include(cmake/SetupDependencies.cmake)

###  Add all source files
add_executable(h5du
        source/main.cpp
        source/tools/tree.cpp
        source/tools/log.cpp
        source/tools/human.cpp
        )
target_include_directories(h5du PRIVATE source)
set_target_properties(h5du PROPERTIES LINK_WHAT_YOU_USE TRUE)

# Link all the things
target_link_libraries(h5du PUBLIC deps flags)
target_link_precompiled_headers(h5du)

# Print summary
include(cmake/PrintTargetInfo.cmake)
print_project_summary(h5du)


# Create an install target
install(TARGETS h5du COMPONENT executable)

# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/h5duUninstall.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/h5duUninstall.cmake
            IMMEDIATE @ONLY)

    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/h5duUninstall.cmake)
endif()





