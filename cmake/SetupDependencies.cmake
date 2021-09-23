
include(cmake/SetupDependenciesCMake.cmake)
include(cmake/SetupDependenciesConan.cmake)

target_link_libraries(project-settings INTERFACE h5pp::h5pp)
