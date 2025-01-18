add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE ${PROJECT_LIBRARY_NAME} ${PROJECT_COMPILER_FLAGS}
)
