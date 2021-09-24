


include(cmake/SetupPaths.cmake) # Setup paths that find_package should search
include(cmake/SetupDependenciesFind.cmake)
include(cmake/SetupDependenciesCMake.cmake)
include(cmake/SetupDependenciesConan.cmake)

if(NOT TARGET deps)
    add_library(deps INTERFACE)
endif()

target_link_libraries(deps INTERFACE h5pp::h5pp)


