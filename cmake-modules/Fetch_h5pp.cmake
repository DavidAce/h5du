if(NOT TARGET h5pp::h5pp AND TB_DOWNLOAD_METHOD STREQUAL "find")
    find_package(h5pp 1.8.0 QUIET NO_CMAKE_PACKAGE_REGISTRY)
    if(h5pp_FOUND AND TARGET h5pp::h5pp)
        message(STATUS "Found h5pp")
    endif()
endif()

if(NOT TARGET h5pp::h5pp AND TB_DOWNLOAD_METHOD MATCHES "fetch")
    find_package(h5pp  1.8.0
            NO_CMAKE_PACKAGE_REGISTRY)
    if(h5pp_FOUND AND TARGET h5pp::h5pp)
        message(STATUS "Found h5pp")
    endif()
endif()

if(NOT TARGET h5pp::h5pp AND TB_DOWNLOAD_METHOD MATCHES "fetch")
    message(STATUS "h5pp will be installed into ${CMAKE_INSTALL_PREFIX}")
    include(${PROJECT_SOURCE_DIR}/cmake-modules/BuildDependency.cmake)
    list(APPEND H5PP_CMAKE_OPTIONS  -DEigen3_ROOT:PATH=${CMAKE_INSTALL_PREFIX}/Eigen3)
    list(APPEND H5PP_CMAKE_OPTIONS  -DH5PP_DOWNLOAD_METHOD:BOOL=${TB_DOWNLOAD_METHOD})
    list(APPEND H5PP_CMAKE_OPTIONS  -DH5PP_PREFER_CONDA_LIBS:BOOL=${TB_PREFER_CONDA_LIBS})
    list(APPEND H5PP_CMAKE_OPTIONS  -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE})
    build_dependency(h5pp "${CMAKE_INSTALL_PREFIX}" "${H5PP_CMAKE_OPTIONS}")
    find_package(h5pp 1.8.0
            NO_CMAKE_PACKAGE_REGISTRY)
    if(h5pp_FOUND AND TARGET h5pp::h5pp)
        message(STATUS "h5pp installed successfully")
    else()
        message(FATAL_ERROR "h5pp could not be installed")
    endif()
endif()