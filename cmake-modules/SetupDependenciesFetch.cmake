if(H5DU_DOWNLOAD_METHOD MATCHES "find|fetch")
    # Let cmake find our Find<package>.cmake modules
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}) # Works like HINTS but can be ignored by NO_DEFAULT_PATH NO_CMAKE_PATH and NO_CMAKE_ENVIRONMENT_PATH
    #list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}) # Works like HINTS but can be ignored by NO_DEFAULT_PATH NO_CMAKE_PATH and NO_CMAKE_ENVIRONMENT_PATH
    #list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_INSTALL_PREFIX}) # Prepends stuff like ${CMAKE_INSTALL_PREFIX} to absolute paths
    if(CMAKE_SIZEOF_VOID_P EQUAL 8 OR CMAKE_GENERATOR MATCHES "64")
        set(FIND_LIBRARY_USE_LIB64_PATHS ON)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(FIND_LIBRARY_USE_LIB32_PATHS ON)
    endif()


    ##############################################################################
    ###  Optional OpenMP support                                               ###
    ###  Note that Clang has some  trouble with static openmp and that         ###
    ###  and that static openmp is not recommended. This tries to enable       ###
    ###  static openmp anyway because I find it useful. Installing             ###
    ###  libiomp5 might help for shared linking.                               ###
    ##############################################################################
    include(cmake-modules/Fetch_h5pp.cmake)                         # h5pp for writing to file binary in format

    ##################################################################
    ### Link all the things!                                       ###
    ##################################################################
    if(TARGET h5pp::h5pp)
        list(APPEND FOUND_TARGETS  h5pp::h5pp)
    endif()
    if(TARGET Threads::Threads)
        list(APPEND FOUND_TARGETS Threads::Threads)
    endif()
    if(FOUND_TARGETS)
        mark_as_advanced(FOUND_TARGETS)
    endif()
endif()