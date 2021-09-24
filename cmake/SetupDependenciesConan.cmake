cmake_minimum_required(VERSION 3.15)

if(H5DU_PACKAGE_MANAGER MATCHES "conan")

    if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        message(STATUS "Detected Conan build info: ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
    else()

        ##################################################################
        ### Install dependencies from conanfile.txt                    ###
        ##################################################################

        find_program (
                CONAN_COMMAND
                conan
                HINTS ${H5DU_CONAN_CANDIDATE_PATHS}
                PATH_SUFFIXES bin envs/dmrg/bin
        )

        if(NOT CONAN_COMMAND)
            message(FATAL_ERROR "Could not find conan program executable")
        else()
            message(STATUS "Found conan: ${CONAN_COMMAND}")
        endif()



        # Download automatically, you can also just copy the conan.cmake file
        if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
            message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
            file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                    "${CMAKE_BINARY_DIR}/conan.cmake")
        endif()

        include(${CMAKE_BINARY_DIR}/conan.cmake)

        if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
            # Let it autodetect libcxx
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            # There is no libcxx
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            list(APPEND conan_libcxx compiler.libcxx=libstdc++11)
        endif()

        conan_cmake_run(
                CONANFILE conanfile.txt
                CONAN_COMMAND ${CONAN_COMMAND}
                SETTINGS compiler.cppstd=17
                SETTINGS "${conan_libcxx}"
                PROFILE_AUTO ALL
                BUILD_TYPE ${CMAKE_BUILD_TYPE}
                BASIC_SETUP CMAKE_TARGETS
                NO_OUTPUT_DIRS
                BUILD missing)
    endif()

    if(TARGET CONAN_PKG::h5pp)
        if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
            add_library(h5pp::h5pp ALIAS CONAN_PKG::h5pp)
        else()
            add_library(h5pp::h5pp INTERFACE IMPORTED)
            target_link_libraries(h5pp::h5pp INTERFACE CONAN_PKG::h5pp)
        endif()
    endif()
endif()
