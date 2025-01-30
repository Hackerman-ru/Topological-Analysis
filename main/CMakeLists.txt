add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARY} ${PROJECT_COMPILER_FLAGS})
target_compile_definitions(${PROJECT_NAME} PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")
