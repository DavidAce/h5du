
if(H5DU_DOWNLOAD_METHOD)
    message(WARNING "The variable [H5DU_DOWNLOAD_METHOD] has been deprecated\n"
            "Use the following variable instead:\n"
            "\t H5DU_PACKAGE_MANAGER:STRING=[find|cmake|find-or-cmake|conan]")
    set(H5DU_PACKAGE_MANAGER ${H5DU_DOWNLOAD_METHOD} CACHE STRING "")
endif()

if(H5DU_DEPS_IN_SUBDIR)
    message(WARNING "The option [H5DU_DEPS_IN_SUBDIR] has been deprecated\n"
            "Use the following variable instead:\n"
            "\t H5DU_PREFIX_ADD_PKGNAME:BOOL=[TRUE|FALSE]")
    set(H5DU_PREFIX_ADD_PKGNAME ${H5DU_DEPS_IN_SUBDIR})
endif()

if(H5DU_PRINT_INFO)
    message(WARNING "The option [H5DU_PRINT_INFO] has been deprecated\n"
            "Use the built-in CMake CLI option --loglevel=[TRACE|DEBUG|VERBOSE|STATUS...] instead")
endif()