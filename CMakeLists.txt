cmake_minimum_required(VERSION 3.14)

project(
    topa
    VERSION 1.0
    LANGUAGES CXX
    DESCRIPTION "Matrix algo lib for C++"
)

add_library(topa INTERFACE)
add_library(topa::topa ALIAS topa)

set_target_properties(topa PROPERTIES LINKER_LANGUAGE CXX)

set(public_headers src/include/topa.h)
include(CMakePackageConfigHelpers)

target_include_directories(topa
  INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        "${CMAKE_CURRENT_SOURCE_DIR}/src")

set_target_properties(topa PROPERTIES
    PUBLIC_HEADER "${public_headers}"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.87.0
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(Boost)

enable_testing()

include(src/CMakeLists.txt)
include(tests/CMakeLists.txt)

target_link_libraries(
  tests
  GTest::gtest_main
  Boost::asio
)

include(GoogleTest)
gtest_discover_tests(tests)
