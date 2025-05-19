cmake_minimum_required(VERSION 3.28)
project(topa VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

include(cmake/Options.cmake)
include(cmake/DownloadDependencies.cmake)
include(cmake/ConfigureTarget.cmake)

setup_dependencies()

add_library(topa)
add_library(topa::topa ALIAS topa)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


configure_main_target(topa)

if(TOPA_BUILD_TESTS)
    configure_tests(topa)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(cmake/InstallConfig.cmake)
endif()
