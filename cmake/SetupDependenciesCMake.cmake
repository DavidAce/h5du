cmake_minimum_required(VERSION 3.15)

if(H5DU_PACKAGE_MANAGER MATCHES "cmake")
    include(cmake/InstallPackage.cmake)
    if(H5DU_PREFIX_ADD_PKGNAME)
        set(INSTALL_PREFIX_PKGNAME INSTALL_PREFIX_PKGNAME)
    endif()

    # h5pp for writing to file binary in format
    install_package(h5pp VERSION 1.9.0 CMAKE_ARGS
            -DEigen3_ROOT:PATH=${H5DU_DEPS_INSTALL_DIR}
            -DH5PP_PACKAGE_MANAGER:STRING=cmake
            -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}
            )

    # Link to h5pp dependencies
    target_link_libraries(deps INTERFACE ${fmt_TARGET} ${spdlog_TARGET} ${Eigen3_TARGET} ${hdf5_TARGET})

endif()
