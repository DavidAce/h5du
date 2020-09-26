cmake_minimum_required(VERSION 3.12)

if(TB_DOWNLOAD_METHOD MATCHES "find")
    if(NOT TB_DOWNLOAD_METHOD MATCHES "fetch")
        set(REQUIRED ${REQUIRED})
    endif()
    if(NOT TARGET spdlog::spdlog)
        # We don't need to find fmt here because
        # spdlog will try to find it in its config script
        find_package(spdlog 1.3.1 ${REQUIRED})
        if(TARGET spdlog)
            add_library(spdlog::spdlog ALIAS spdlog)
        endif()
    endif()
endif()



# Download spdlog
if (TB_DOWNLOAD_METHOD MATCHES "fetch" AND NOT TARGET spdlog::spdlog)
    find_package(spdlog 1.3
            HINTS ${spdlog_ROOT} ${CMAKE_INSTALL_PREFIX}
            NO_DEFAULT_PATH)
    if(NOT TARGET spdlog::spdlog)
        message(STATUS "Spdlog will be installed into ${CMAKE_INSTALL_PREFIX}")
        if(TARGET fmt::fmt)
            get_target_property(FMT_INC  fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
            get_target_property(FMT_LOC  fmt::fmt LOCATION)
            get_filename_component(fmt_ROOT ${FMT_INC}/.. ABSOLUTE)
            mark_as_advanced(FMT_LOC)
            mark_as_advanced(FMT_INC)
            list(APPEND SPDLOG_CMAKE_OPTIONS  "-DSPDLOG_FMT_EXTERNAL:BOOL=ON")
            list(APPEND SPDLOG_CMAKE_OPTIONS  "-Dfmt_ROOT:PATH=${fmt_ROOT}")
        endif()
        include(${PROJECT_SOURCE_DIR}/cmake-modules/BuildDependency.cmake)
        build_dependency(spdlog  "${CMAKE_INSTALL_PREFIX}" "${SPDLOG_CMAKE_OPTIONS}")
        find_package(spdlog 1.3
                HINTS ${spdlog_ROOT} ${CMAKE_INSTALL_PREFIX}
                NO_DEFAULT_PATH
                REQUIRED)
        if(TARGET spdlog::spdlog)
            message(STATUS "spdlog installed successfully")
        endif()
    endif()
    if(TARGET spdlog::spdlog)
        list(APPEND H5PP_TARGETS spdlog::spdlog)
        if(TARGET fmt::fmt)
            target_link_libraries(spdlog::spdlog INTERFACE fmt::fmt)
        else()
            message(FATAL_ERROR "Missing target fmt::fmt is required for Spdlog")
        endif()
    else()
        message(FATAL_ERROR "Spdlog could not be downloaded and built from source")
    endif()
endif()
