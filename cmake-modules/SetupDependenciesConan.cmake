
if(H5DU_DOWNLOAD_METHOD MATCHES "conan")

    if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        message(STATUS "Detected Conan build info: ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_basic_setup(TARGETS)
        ##################################################################
        ### Link all the things!                                       ###
        ##################################################################
    else()

        ##################################################################
        ### Install conan-modules/conanfile.txt dependencies          ###
        ### This uses conan to get spdlog,eigen3,h5pp,ceres-solver    ###
        ###    h5pp/1.7.3@davidace/stable                             ###
        ##################################################################

        find_program (
                CONAN_COMMAND
                conan
                HINTS ${CONAN_PREFIX} $ENV{CONAN_PREFIX} ${CONDA_PREFIX} $ENV{CONDA_PREFIX}
                PATHS $ENV{HOME}/anaconda3  $ENV{HOME}/miniconda3 $ENV{HOME}/anaconda $ENV{HOME}/miniconda $ENV{HOME}/.conda
                PATH_SUFFIXES bin envs/tb/bin  envs/dmrg/bin
        )
        if(NOT CONAN_COMMAND)
            message(FATAL_ERROR "Could not find conan program executable")
        else()
            message(STATUS "Found conan: ${CONAN_COMMAND}")
        endif()

        # Download cmake-conan automatically, you can also just copy the conan.cmake file
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
            list(APPEND H5DU_CONAN_SETTINGS SETTINGS compiler.libcxx=libstdc++11)
        endif()
        conan_cmake_run(
                CONANFILE conanfile.txt
                CONAN_COMMAND ${CONAN_COMMAND}
                BUILD_TYPE ${CMAKE_BUILD_TYPE}
                BASIC_SETUP CMAKE_TARGETS
                SETTINGS compiler.cppstd=17
                SETTINGS compiler.libcxx=libstdc++11
                ${H5DU_CONAN_SETTINGS}
                ${H5DU_CONAN_OPTIONS}
                BUILD missing
        )
    endif()

    list(APPEND H5DU_TARGETS ${CONAN_TARGETS})
endif()
