function(configure_main_target TARGET_NAME)
    # Источники и инклуды
    file(GLOB_RECURSE TOPA_SOURCES "src/*.cpp")
    target_sources(${TARGET_NAME} PRIVATE ${TOPA_SOURCES})

    target_include_directories(${TARGET_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    # Фичи
    if(TOPA_ENABLE_EIGEN)
        target_compile_definitions(${TARGET_NAME} PUBLIC TOPA_USE_EIGEN)
        target_link_libraries(${TARGET_NAME} PUBLIC Eigen3::Eigen Spectra::Spectra)
    endif()

    if(TOPA_ENABLE_TBB)
        target_compile_definitions(${TARGET_NAME} PUBLIC TOPA_USE_TBB)
        target_link_libraries(${TARGET_NAME} PUBLIC TBB::tbb)
    endif()

    set_property(TARGET ${TARGET_NAME} PROPERTY EXPORT_PROPERTIES 
        "TOPA_ENABLE_EIGEN;TOPA_ENABLE_TBB"
    )

    # Clang-tidy
    if(TOPA_ENABLE_CLANG_TIDY)
        find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
        if(CLANG_TIDY_EXE)
            set_target_properties(${TARGET_NAME} PROPERTIES
                CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
            )
        else()
            message(WARNING "clang-tidy not found, static analysis disabled")
        endif()
    endif()
endfunction()

function(configure_tests MAIN_TARGET)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.4
    )
    FetchContent_MakeAvailable(Catch2)

    # Test main
    enable_testing()
    add_library(catch_main tools/catch_main.cpp)
    target_link_libraries(catch_main PUBLIC Catch2::Catch2)

    # Санитайзеры
    set(SANITIZER_FLAGS)
    if(TOPA_ENABLE_ASAN)
        set(SANITIZER_FLAGS -fsanitize=address -fno-omit-frame-pointer)
    elseif(TOPA_ENABLE_TSAN)
        set(SANITIZER_FLAGS -fsanitize=thread -fno-omit-frame-pointer)
    endif()

    # Сборка тестов
    file(GLOB TEST_SOURCES "tests/*.cpp")
    foreach(test_source ${TEST_SOURCES})
        get_filename_component(test_name ${test_source} NAME_WE)
        add_executable(${test_name} ${test_source})

        target_link_libraries(${test_name}
            PRIVATE
                ${MAIN_TARGET}
                catch_main
        )

        target_compile_definitions(${test_name} PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")

        if(SANITIZER_FLAGS)
            target_compile_options(${test_name} PRIVATE ${SANITIZER_FLAGS})
            target_link_options(${test_name} PRIVATE ${SANITIZER_FLAGS})
        endif()

        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endfunction()
