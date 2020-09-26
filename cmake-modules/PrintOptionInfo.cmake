if(H5DU_PRINT_INFO)
    # Print CMake options
    message(STATUS  "|----------------\n"
            "-- | BUILD_SHARED_LIBS       : ${BUILD_SHARED_LIBS}\n"
            "-- | CMAKE_INSTALL_PREFIX    : ${CMAKE_INSTALL_PREFIX}\n"
            "-- | H5DU_BUILD_EXAMPLES     : ${H5DU_BUILD_EXAMPLES}\n"
            "-- | H5DU_ENABLE_TESTS       : ${H5DU_ENABLE_TESTS}\n"
            "-- | H5DU_ENABLE_LTO         : ${H5DU_ENABLE_LTO}\n"
            "-- | H5DU_ENABLE_ASAN        : ${H5DU_ENABLE_ASAN}\n"
            "-- | H5DU_DOWNLOAD_METHOD    : ${H5DU_DOWNLOAD_METHOD}\n"
            "-- | H5DU_PREFER_CONDA_LIBS  : ${H5DU_PREFER_CONDA_LIBS}\n"
            "-- | H5DU_PRINT_INFO         : ${H5DU_PRINT_INFO}\n")
endif ()
