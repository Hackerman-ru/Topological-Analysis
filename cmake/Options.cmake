# Основные опции проекта
option(TOPA_ENABLE_EIGEN "Enable Eigen support" ON)
option(TOPA_ENABLE_TBB "Enable Intel TBB support" ON)
option(TOPA_BUILD_TESTS "Build tests" ON)
option(TOPA_ENABLE_CLANG_TIDY "Enable clang-tidy checks" ON)

# Санитайзеры
option(TOPA_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(TOPA_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

# Проверка конфликтов санитайзеров
if(TOPA_ENABLE_ASAN AND TOPA_ENABLE_TSAN)
    message(FATAL_ERROR "ASan and TSan cannot be enabled simultaneously")
endif()
