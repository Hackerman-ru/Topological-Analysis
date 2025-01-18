cmake_minimum_required(VERSION 3.23)

# set the project name, version and description
project(
    topa
    VERSION 1.0
    LANGUAGES CXX
    DESCRIPTION "Topological Analysis library"
)

set(PROJECT_COMPILER_FLAGS ${PROJECT_NAME}_compiler_flags)
set(PROJECT_LIBRARY ${PROJECT_NAME}_lib)
set(FETCHCONTENT_QUIET FALSE)
set(INSTALL_GTEST OFF)

# add compiler flags interface and specify the C++ standard
add_library(${PROJECT_COMPILER_FLAGS} INTERFACE)
target_compile_features(${PROJECT_COMPILER_FLAGS} INTERFACE cxx_std_20)

# add compiler warning flags just when building this project via
# the BUILD_INTERFACE genex
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(${PROJECT_COMPILER_FLAGS} INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

# control where the static and shared libraries are built so that on windows
# we don't need to tinker with the path to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# add uninstall target if it has not yet been defined
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

include(FetchContent)

# add the project library
add_subdirectory(src)

# add the tests
add_subdirectory(tests)

# add the executable
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIBRARY_NAME} ${PROJECT_COMPILER_FLAGS})
