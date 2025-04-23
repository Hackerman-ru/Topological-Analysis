function(setup_dependencies)
    include(FetchContent)

    # TBB
    if(TOPA_ENABLE_TBB)
        find_package(TBB QUIET)
        if(NOT TBB_FOUND)
            FetchContent_Declare(
                tbb SYSTEM
                GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
                GIT_TAG        v2022.1.0
                GIT_SHALLOW TRUE
            )
            FetchContent_MakeAvailable(tbb)
        endif()
        setup_tbb_target()
    endif()

    # Eigen3 + Spectra
    if(TOPA_ENABLE_EIGEN)
        set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
        set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
        set(EIGEN_TEST_CXX11 OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            eigen SYSTEM
            URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
            GIT_SHALLOW TRUE
            SOURCE_SUBDIR src
        )
        FetchContent_Declare(
            spectra SYSTEM
            GIT_REPOSITORY https://github.com/yixuan/spectra.git
            GIT_TAG        v1.1.0
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(eigen spectra)
        setup_eigen_target()
        setup_spectra_target()
    endif()

    # Clang
    set(Clang_DIR "/usr/lib/llvm-19/lib/cmake/clang")
    find_package(Clang CONFIG)
endfunction()

function(setup_tbb_target)
    if(NOT TARGET TBB::tbb)
        add_library(TBB::tbb INTERFACE IMPORTED)
        target_link_libraries(TBB::tbb INTERFACE tbb)
    endif()
endfunction()

function(setup_eigen_target)
    if(NOT TARGET Eigen3::Eigen)
        add_library(Eigen3::Eigen INTERFACE IMPORTED)
        target_include_directories(Eigen3::Eigen INTERFACE ${eigen_SOURCE_DIR})
    endif()
endfunction()

function(setup_spectra_target)
    if(NOT TARGET Spectra::Spectra)
        add_library(Spectra::Spectra INTERFACE IMPORTED)
        target_include_directories(Spectra::Spectra INTERFACE ${spectra_SOURCE_DIR}/include)
        target_link_libraries(Spectra::Spectra INTERFACE Eigen3::Eigen)
    endif()
endfunction()
