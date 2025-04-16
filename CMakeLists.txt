cmake_minimum_required(VERSION 3.28)
project(topa LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TOPA_ENABLE_EIGEN "Enable Eigen support" OFF)
option(TOPA_ENABLE_TBB "Enable Intel TBB support" OFF)

if(TOPA_ENABLE_TBB)
    find_package(TBB REQUIRED)
endif()

if(TOPA_ENABLE_EIGEN)
    find_package(Eigen3 REQUIRED)
endif()

set(Clang_DIR "/usr/lib/llvm-19/lib/cmake/clang")
find_package(Clang CONFIG)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(topa)
add_library(topa::topa ALIAS topa)

if(TOPA_ENABLE_EIGEN)
    target_compile_definitions(topa PUBLIC TOPA_USE_EIGEN)
endif()

if(TOPA_ENABLE_TBB)
    target_compile_definitions(topa PUBLIC TOPA_USE_TBB)
endif()

option(TOPA_ENABLE_CLANG_TIDY "Enable clang-tidy checks" ON)
if(TOPA_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set_target_properties(topa PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
        )
    else()
        message(WARNING "clang-tidy not found, static analysis disabled")
    endif()
endif()

target_include_directories(topa
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/topa>
        $<INSTALL_INTERFACE:include>
)

file(GLOB_RECURSE TOPA_SOURCES "src/*.cpp")
target_sources(topa PRIVATE ${TOPA_SOURCES})

if(TOPA_ENABLE_EIGEN)
    target_link_libraries(topa PUBLIC Eigen3::Eigen)
    if(TARGET Eigen3::Eigen)
        target_include_directories(topa SYSTEM PUBLIC
            $<TARGET_PROPERTY:Eigen3::Eigen,INTERFACE_INCLUDE_DIRECTORIES>
        )
    endif()
endif()

if(TOPA_ENABLE_TBB)
    target_link_libraries(topa PUBLIC TBB::tbb)
    if(TARGET TBB::tbb)
        target_include_directories(topa SYSTEM PUBLIC
            $<TARGET_PROPERTY:TBB::tbb,INTERFACE_INCLUDE_DIRECTORIES>
        )
    endif()
endif()

option(TOPA_BUILD_TESTS "Build tests" ON)

if(TOPA_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.4
    )
    FetchContent_MakeAvailable(Catch2)

    set(TEST_CLANG_TIDY "")
    if(TOPA_ENABLE_CLANG_TIDY)
        set(TEST_CLANG_TIDY "")
    endif()

    add_library(catch_main tools/catch_main.cpp)
    target_link_libraries(catch_main PUBLIC Catch2::Catch2)

    option(TOPA_ENABLE_ASAN "Enable AddressSanitizer" OFF)
    option(TOPA_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

    if(TOPA_ENABLE_ASAN AND TOPA_ENABLE_TSAN)
        message(FATAL_ERROR "ASan and TSan cannot be enabled simultaneously")
    endif()

    set(SANITIZER_FLAGS)
    if(TOPA_ENABLE_ASAN)
        set(SANITIZER_FLAGS -fsanitize=address -fno-omit-frame-pointer)
    elseif(TOPA_ENABLE_TSAN)
        set(SANITIZER_FLAGS -fsanitize=thread -fno-omit-frame-pointer)
    endif()

    file(GLOB TEST_SOURCES "tests/*.cpp")
    foreach(test_source ${TEST_SOURCES})
        get_filename_component(test_name ${test_source} NAME_WE)
        add_executable(${test_name} ${test_source})
        target_link_libraries(${test_name}
            PRIVATE
                topa::topa
                catch_main
        )

        target_compile_definitions(${test_name} PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")

        if(SANITIZER_FLAGS)
            target_compile_options(${test_name} PRIVATE ${SANITIZER_FLAGS})
            target_link_options(${test_name} PRIVATE ${SANITIZER_FLAGS})
        endif()

        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()
