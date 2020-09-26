# Download fmt
if (TB_DOWNLOAD_METHOD MATCHES "fetch" AND NOT TARGET acrotensor::acrotensor_static)
    find_package(acrotensor
            HINTS ${acrotensor_ROOT} ${CMAKE_INSTALL_PREFIX}
            NO_DEFAULT_PATH)
    if(NOT TARGET acrotensor::acrotensor_static)
        message(STATUS "acrotensor will be installed into ${CMAKE_INSTALL_PREFIX}")
        list(APPEND ACROTENSOR_CMAKE_OPTIONS  "-DACROTENSOR_ENABLE_CUDA:BOOL=${TB_ENABLE_CUDA}")
        include(${PROJECT_SOURCE_DIR}/cmake-modules/BuildDependency.cmake)
        build_dependency(acrotensor  "${CMAKE_INSTALL_PREFIX}" "${ACROTENSOR_CMAKE_OPTIONS}")
        find_package(acrotensor
                HINTS ${acrotensor_ROOT} ${CMAKE_INSTALL_PREFIX}
                NO_DEFAULT_PATH
                REQUIRED)

        # Print summary of CMake configuration
        if (TB_PRINT_INFO)

        endif()
        include(cmake-modules/PrintTargetInfo.cmake)
        print_target_info(acrotensor "| ")
        print_target_info(acrotensor_static "| ")
        print_target_info(acrotensor::acrotensor_static "| ")
        print_target_info(acrotensor_shared "| ")
        print_target_info(acrotensor::acrotensor_shared "| ")

        if(TARGET acrotensor::acrotensor)
            message(STATUS "acrotensor installed successfully")
        else()
            message(FATAL_ERROR "acrotensor could not be downloaded and built from source")
        endif()
    endif()
endif()