enable_testing()

# Download and install GoogleTest
FetchContent_Declare(
    gtest
    URL https://github.com/google/googletest/archive/504ea69cf7e9947be54f808a09b7b08988e84b5f.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)
FetchContent_MakeAvailable(gtest)

include(GoogleTest)

# discover and add Tests
add_executable(Tests test_pairing.cpp)
target_link_libraries(Tests ${PROJECT_LIBRARY} gtest_main ${PROJECT_COMPILER_FLAGS})
gtest_discover_tests(Tests)
