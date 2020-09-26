# Download fmt
if (TB_DOWNLOAD_METHOD MATCHES "fetch" AND NOT TARGET fmt::fmt AND NOT TARGET spdlog::spdlog)
    # fmt is a dependency of spdlog
    # We fetch it here to get the latest version and to make sure we use the
    # compile library and avoid compile-time overhead in projects consuming h5pp.
    # Note that spdlog may already have been found in if TB_DOWNLOAD_METHOD=find|fetch
    # then we can assume that spdlog already knows how and where to get fmt.
    find_package(fmt 6.2.1
            HINTS ${fmt_ROOT} ${CMAKE_INSTALL_PREFIX}
            NO_DEFAULT_PATH)
    if(NOT TARGET fmt::fmt)
        message(STATUS "fmt will be installed into ${CMAKE_INSTALL_PREFIX}")
        list(APPEND FMT_CMAKE_OPTIONS  "-DFMT_TEST:BOOL=OFF")
        list(APPEND FMT_CMAKE_OPTIONS  "-DFMT_DOC:BOOL=OFF")
        include(${PROJECT_SOURCE_DIR}/cmake-modules/BuildDependency.cmake)
        build_dependency(fmt  "${CMAKE_INSTALL_PREFIX}" "${FMT_CMAKE_OPTIONS}")
        find_package(fmt 6.2.1
                HINTS ${fmt_ROOT} ${CMAKE_INSTALL_PREFIX}
                NO_DEFAULT_PATH
                REQUIRED)
        if(TARGET fmt::fmt)
            message(STATUS "fmt installed successfully")
        else()
            message(FATAL_ERROR "fmt could not be downloaded and built from source")
        endif()
    endif()
endif()