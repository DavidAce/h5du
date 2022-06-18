cmake_minimum_required(VERSION 3.15)

if(H5DU_PACKAGE_MANAGER MATCHES "cmake")
    include(cmake/InstallPackage.cmake)

    list(APPEND h5pp_ARGS -DEigen3_ROOT:PATH=${DMRG_DEPS_INSTALL_DIR})
    list(APPEND h5pp_ARGS -DH5PP_PACKAGE_MANAGER:STRING=cmake)
    list(APPEND h5pp_ARGS -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE})

    # h5pp for writing to file binary in format
    install_package(h5pp VERSION 1.10.0
            CMAKE_ARGS
                -DH5PP_PACKAGE_MANAGER:STRING=cmake
                -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}
            )
    find_package(h5pp         1.10.0 REQUIRED CONFIG)

endif()
