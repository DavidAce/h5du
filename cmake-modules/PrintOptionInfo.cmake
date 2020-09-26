if(TB_PRINT_INFO)
    # Print CMake options
    message(STATUS  "|----------------\n"
            "-- | BUILD_SHARED_LIBS       : ${BUILD_SHARED_LIBS}\n"
            "-- | CMAKE_INSTALL_PREFIX    : ${CMAKE_INSTALL_PREFIX}\n"
            "-- | TB_BUILD_EXAMPLES     : ${TB_BUILD_EXAMPLES}\n"
            "-- | TB_ENABLE_TESTS       : ${TB_ENABLE_TESTS}\n"
            "-- | TB_ENABLE_OPENMP      : ${TB_ENABLE_OPENMP}\n"
            "-- | TB_ENABLE_MKL         : ${TB_ENABLE_MKL}\n"
            "-- | TB_ENABLE_LTO         : ${TB_ENABLE_LTO}\n"
            "-- | TB_ENABLE_ASAN        : ${TB_ENABLE_ASAN}\n"
            "-- | TB_ENABLE_CUDA        : ${TB_ENABLE_CUDA}\n"
            "-- | TB_DOWNLOAD_METHOD    : ${TB_DOWNLOAD_METHOD}\n"
            "-- | TB_PREFER_CONDA_LIBS  : ${TB_PREFER_CONDA_LIBS}\n"
            "-- | TB_PRINT_INFO         : ${TB_PRINT_INFO}\n")
endif ()
