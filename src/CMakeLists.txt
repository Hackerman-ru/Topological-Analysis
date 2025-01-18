set(
    Sources
    vertices/base.cpp
    vertices/point.cpp
    fast_generator.cpp
    index_matrix.cpp
)

set(
    Headers
    ${PROJECT_SOURCE_DIR}/include/topa/core/complex.h
    ${PROJECT_SOURCE_DIR}/include/topa/core/persistence_pairing.h
    ${PROJECT_SOURCE_DIR}/include/topa/core/simplex.h
    ${PROJECT_SOURCE_DIR}/include/topa/core/util.h
    ${PROJECT_SOURCE_DIR}/include/topa/vertices/base.h
    ${PROJECT_SOURCE_DIR}/include/topa/vertices/point.h
    ${PROJECT_SOURCE_DIR}/include/topa/fast_generator.h
    ${PROJECT_SOURCE_DIR}/include/topa/index_matrix.h
)

# add the library and set the include headers
add_library(${PROJECT_LIBRARY} STATIC ${Sources})
target_include_directories(
    ${PROJECT_LIBRARY} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_sources(${PROJECT_LIBRARY} PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/include
    FILES "${Headers}"
)

# Fetch and make available boost
FetchContent_Declare(
  boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_PROGRESS TRUE
  GIT_TAG boost-1.87.0
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(boost)

# link library to boost and compiler_flags
target_link_libraries(
    ${PROJECT_LIBRARY}
    PRIVATE Boost::asio
    PUBLIC ${PROJECT_COMPILER_FLAGS}
)

# install lib
install(TARGETS ${PROJECT_LIBRARY}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    EXPORT ${PROJECT_LIBRARY}Targets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
