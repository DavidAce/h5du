
if(H5DU_PACKAGE_MANAGER MATCHES "find")
    if(H5DU_PACKAGE_MANAGER STREQUAL "find")
        set(REQUIRED REQUIRED)
    endif()
    # Start finding the dependencies
    find_package(h5pp 1.9.0 ${REQUIRED})
endif()