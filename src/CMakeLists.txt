set(HEADERS_DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME})

set(
    Headers
    ${HEADERS_DIRECTORY}/bit_tree_index_column.hpp
    ${HEADERS_DIRECTORY}/complex.hpp
    ${HEADERS_DIRECTORY}/filtration.hpp
    ${HEADERS_DIRECTORY}/off_reader.hpp
    ${HEADERS_DIRECTORY}/persistence_pairing.hpp
    ${HEADERS_DIRECTORY}/simplex.hpp
    ${HEADERS_DIRECTORY}/twist_reducer.hpp
    ${HEADERS_DIRECTORY}/vertex.hpp
    ${HEADERS_DIRECTORY}/vietoris_rips.hpp
    ${HEADERS_DIRECTORY}/topa.hpp
)

# add the library and set the include headers
add_library(${PROJECT_LIBRARY} INTERFACE ${Headers})
target_include_directories(
    ${PROJECT_LIBRARY} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_sources(
    ${PROJECT_LIBRARY} PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/include
    FILES "${Headers}"
)

# link library to boost and compiler_flags
target_link_libraries(${PROJECT_LIBRARY} INTERFACE ${PROJECT_COMPILER_FLAGS})

# install lib
install(
    TARGETS ${PROJECT_LIBRARY}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    EXPORT ${PROJECT_LIBRARY}Targets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
