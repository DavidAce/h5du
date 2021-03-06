cmake_minimum_required(VERSION 3.12)
project(h5du
        VERSION 0.0.1
        DESCRIPTION "HDF5 Disk Usage"
        HOMEPAGE_URL "")


# Make sure we use DMRG's own find modules
list(INSERT CMAKE_MODULE_PATH 0  ${PROJECT_SOURCE_DIR}/cmake-modules)

# Set options this build
option(H5DU_PREFER_CONDA_LIBS        "Prefer libraries installed through anaconda"                  OFF)
option(H5DU_ENABLE_LTO               "Enable link time optimization"                                OFF)
option(H5DU_ENABLE_TESTS             "Enable unit testing with ctest"                               OFF)
option(H5DU_BUILD_EXAMPLES           "Build examples"                                               OFF)
option(H5DU_PRINT_INFO               "Print information during cmake configure"                     OFF)
option(H5DU_DEPS_IN_SUBDIR           "Install dependencies into CMAKE_INSTALL_PREFIX/<libname>"     OFF)
option(H5DU_ENABLE_ASAN              "Enable runtime address sanitizer -fsanitize=address"          OFF)


# Make an "enum" for valid download methods
set(H5DU_DOWNLOAD_METHODS_VALID find fetch find-or-fetch conan)
set(H5DU_DOWNLOAD_METHOD find CACHE STRING "Download method for external dependencies")
set_property(CACHE H5DU_DOWNLOAD_METHOD PROPERTY STRINGS ${H5DU_DOWNLOAD_METHODS_VALID})
if (NOT H5DU_DOWNLOAD_METHOD IN_LIST H5DU_DOWNLOAD_METHODS_VALID)
    message(FATAL_ERROR "H5DU_DOWNLOAD_METHOD must be one of ${H5DU_DOWNLOAD_METHODS_VALID}")
endif ()




# Set default policies if CMake is new enough
if (CMAKE_VERSION VERSION_LESS 3.12)
    message(STATUS "Not setting policies")
else()
    cmake_policy(SET CMP0074 NEW) # Use <PackageName>_ROOT variables in find_package calls
    cmake_policy(SET CMP0075 NEW)
    # Use cmake_policy CMP0067 to let try_compile honor current CMAKE_CXX_STANDARD setting.
    # See here https://stackoverflow.com/questions/47213356/cmake-using-corrext-c-standard-when-checking-for-header-files
    cmake_policy(SET CMP0067 NEW)
endif()



################################################################
### Get git version number                                   ###
### Generates a header gitversion/gitversion.h               ###
### Include it using #include <gitversion.h>                 ###
### Gives a namespace GIT:: with several git version numbers.###
################################################################
include(cmake-modules/gitversion.cmake)


##################################################################
### Setup dependency paths                                     ###
##################################################################
include(GNUInstallDirs)
string(TOLOWER ${CMAKE_BUILD_TYPE} cmake_build_type)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(STATUS "Setting default install prefix -- CMAKE_INSTALL_PREFIX --> ${CMAKE_BINARY_DIR}/h5du-deps-install")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/h5du-deps-install" CACHE PATH "..." FORCE)
endif()



############################################################
### Print extra info during CMake configure              ###
############################################################
include(cmake-modules/PrintHostInfo.cmake)
include(cmake-modules/PrintOptionInfo.cmake)

###########################################
###  Apply RELEASE/DEBUG compile flags  ###
###########################################
include(cmake-modules/CompilerFlags.cmake)



################################
###  Add all source files    ###
################################
add_executable(${PROJECT_NAME}
        source/main.cpp
        source/tools/tree.cpp
        source/tools/log.cpp
        source/tools/human.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE source)
add_library(project-settings INTERFACE)
##################################################################
###  Force cmake to find .a library suffixes on static builds  ###
##################################################################
if(BUILD_SHARED_LIBS)
    set(LINK_TYPE SHARED)
else()
    set(LINK_TYPE STATIC)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX} )
#    target_link_libraries  (project-settings INTERFACE -static)   ### Static linkage
endif()




##############################
###  Enable c++17 support  ###
##############################
#target_compile_features(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:cxx_std_17>)
#target_compile_features(project-settings INTERFACE $<$<COMPILE_LANGUAGE:CXX>: -std=c++17>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)



##################################################
###  Add optional RELEASE/DEBUG compile flags  ###
##################################################
# -fsanitize=leak -fsanitize=undefined -fsanitize=alignment  -fsanitize=leak -fsanitize=undefined -fsanitize=alignment
target_compile_options(project-settings INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Wall>)
target_compile_options(project-settings INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
target_compile_options(project-settings INTERFACE $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELEASE>>:-g  -mfma>)
target_compile_options(project-settings INTERFACE $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:DEBUG>>: -Wpedantic -Wextra -Wconversion>)
target_compile_options(project-settings INTERFACE $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RELWITHDEBINFO>>:>)
target_compile_options(project-settings INTERFACE $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:MINSIZEREL>>:>)



#######################################
# Settings for sanitizers           ###
#######################################
if(H5DU_ENABLE_ASAN)
    target_compile_options(project-settings INTERFACE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(project-settings INTERFACE -fsanitize=address)
endif()

#######################################
### Compiler-dependent linker flags ###
#######################################
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
    target_link_libraries(project-settings INTERFACE  -stdlib=libstdc++ )
endif()


##################################################################
### Check if we can enable Link Time Optimization              ###
##################################################################
if(H5DU_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT supported OUTPUT error)
    if(supported)
        message(STATUS "IPO / LTO supported, enabling")
        set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
    else()
        message(STATUS "IPO / LTO not supported: <${error}>, will not enable")
        set(H5DU_ENABLE_LTO OFF CACHE)
    endif()
endif()





#################################################################
### Prepend conda dirs for hints when using find_package      ###
#################################################################
if(H5DU_PREFER_CONDA_LIBS)
    list(APPEND CONDA_HINTS
            $ENV{CONDA_PREFIX}
            $ENV{HOME}/anaconda3
            $ENV{HOME}/miniconda
            )
endif()



###########################################
### Fetch or find all the dependencies  ###
###########################################
include(cmake-modules/SetupDependenciesFind.cmake)
include(cmake-modules/SetupDependenciesFetch.cmake)
include(cmake-modules/SetupDependenciesConan.cmake)

##################################################################
### Link all the things!                                       ###
##################################################################
#target_link_libraries(project-settings INTERFACE /home/david/.conan/data/fmt/6.0.0/bincrafters/stable/package/83b377af2053f637540e812f25798c29f53e4921/lib/libfmt.a)
#
target_link_libraries(project-settings INTERFACE  ${CONAN_TARGETS} ${FOUND_TARGETS}) # Order is important here

if(NOT TARGET openmp::openmp)
    target_compile_options(project-settings INTERFACE -Wno-unknown-pragmas)
endif()

target_link_libraries(project-settings INTERFACE -Wl,--whole-archive pthread -Wl,--no-whole-archive -lrt -ldl )
target_link_libraries(${PROJECT_NAME} PRIVATE project-settings )  # Link everything to main project


# Speed up with precompiled headers
if(COMMAND target_precompile_headers)
    target_precompile_headers(${PROJECT_NAME} PRIVATE <vector> <array> <complex> <map> <optional> <iostream> <iomanip> <memory>)
endif()


# Print summary of CMake configuration
if (H5DU_PRINT_INFO)
    include(cmake-modules/PrintTargetInfo.cmake)
    include(cmake-modules/getExpandedTarget.cmake)
    expand_target_all_targets(project-settings H5DU_TARGETS_EXPANDED)
    message(STATUS "| TB TARGET SUMMARY")
    message(STATUS "|--------------------")
    print_compiler_info("| ")
    foreach(tgt ${H5DU_TARGETS_EXPANDED})
        print_target_info(${tgt} "| ")
    endforeach()
endif()



###################
### INFORMATION ###
###################
message("")
message("=============================================================================")
message("")
message("      To clean cmake files, run './build.sh -c'")
message("      To see more options,  run './build.sh -h'")
message("")
message("=============================================================================")
message("")


# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/h5duUninstall.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/h5duUninstall.cmake
            IMMEDIATE @ONLY)

    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/h5duUninstall.cmake)
endif()

#############################
## UNIT TEST
#############################
# Simple testing with ctest
#if (H5DU_ENABLE_TESTS)
#    enable_testing()
#    add_subdirectory(tests)
#endif ()





