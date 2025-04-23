cmake_minimum_required(VERSION 3.28)
project(topa LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

include(cmake/Options.cmake)
include(cmake/DownloadDependencies.cmake)
include(cmake/ConfigureTarget.cmake)

setup_dependencies()

add_library(topa)
add_library(topa::topa ALIAS topa)

configure_main_target(topa)

if(TOPA_BUILD_TESTS)
    configure_tests(topa)
endif()
