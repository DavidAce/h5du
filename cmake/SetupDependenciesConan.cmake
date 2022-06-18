
if(H5DU_PACKAGE_MANAGER MATCHES "conan")


    unset(CONAN_COMMAND CACHE)
    find_program (CONAN_COMMAND conan
            HINTS ${H5DU_CONAN_HINTS}
            PATH_SUFFIXES ${H5DU_CONAN_PATH_SUFFIXES}
            REQUIRED)

    # Download cmake-conan integrator
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan/conan.cmake"
                EXPECTED_HASH MD5=81d5eab13a49f43527e35a90bfac6960
                TLS_VERIFY ON)
    endif()
    include(${CMAKE_BINARY_DIR}/conan/conan.cmake)


    conan_cmake_autodetect(CONAN_AUTODETECT)
    conan_cmake_install(
            CONAN_COMMAND ${CONAN_COMMAND}
            BUILD missing outdated cascade
            GENERATOR CMakeDeps
            SETTINGS ${CONAN_AUTODETECT}
            INSTALL_FOLDER ${CMAKE_BINARY_DIR}/conan
            ENV CC=${CMAKE_C_COMPILER} # Fixes issue with CMake not detecting the right compiler when not building from scratch
            ENV CXX=${CMAKE_CXX_COMPILER} # Fixes issue with CMake not detecting the right compiler when not building from scratch
            PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}
    )

    ##################################################################
    ### Find all the things!                                       ###
    ##################################################################
    if(NOT CONAN_CMAKE_SILENT_OUTPUT)
        set(CONAN_CMAKE_SILENT_OUTPUT OFF) # Default is off
    endif()
    list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/conan)
    list(PREPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/conan)
    # Use CONFIG to avoid MODULE mode. This is recommended for the cmake_find_package_multi generator
    find_package(h5pp         1.10.0 REQUIRED CONFIG)

endif()
