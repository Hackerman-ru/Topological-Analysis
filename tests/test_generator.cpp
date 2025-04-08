#include "generator.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <vector>

using namespace topa::detail;

Generator<int> generate_numbers(int count) {
    for (int i = 0; i < count; ++i) {
        co_yield i;
    }
}

Generator<int> generator_with_exception() {
    co_yield 1;
    throw std::runtime_error("Test error");
    co_yield 2;
}

TEST_CASE("Empty generator", "[Generator]") {
    Generator<int> empty_gen;
    REQUIRE(empty_gen.begin() == std::default_sentinel);
}

TEST_CASE("Single value generation", "[Generator]") {
    auto gen = generate_numbers(1);
    auto it = gen.begin();

    REQUIRE(*it == 0);
    ++it;
    REQUIRE(it == std::default_sentinel);
}

TEST_CASE("Multiple values generation", "[Generator]") {
    auto gen = generate_numbers(3);
    std::vector<int> result;

    for (int val : gen) {
        result.push_back(val);
    }

    REQUIRE(result == std::vector<int> {0, 1, 2});
}

TEST_CASE("Move semantics", "[Generator]") {
    auto gen1 = generate_numbers(3);
    auto gen2 = std::move(gen1);

    REQUIRE(gen1.begin() == std::default_sentinel);

    std::vector<int> values;
    for (int val : gen2) {
        values.push_back(val);
    }
    REQUIRE(values == std::vector<int> {0, 1, 2});
}

TEST_CASE("Iterator operations", "[Generator]") {
    auto gen = generate_numbers(2);
    auto it = gen.begin();

    REQUIRE(*it == 0);
    REQUIRE(it != std::default_sentinel);

    ++it;
    REQUIRE(*it == 1);
    REQUIRE(it != std::default_sentinel);

    ++it;
    REQUIRE(it == std::default_sentinel);
}

TEST_CASE("Exception handling", "[Generator]") {
    auto gen = generator_with_exception();
    auto it = gen.begin();

    REQUIRE(*it == 1);
    REQUIRE_THROWS_AS(++it, std::runtime_error);
}

TEST_CASE("Range-based for loop", "[Generator]") {
    std::vector<int> result;
    for (int val : generate_numbers(3)) {
        result.push_back(val);
    }
    REQUIRE(result == std::vector<int> {0, 1, 2});
}

TEST_CASE("Move-only type support", "[Generator]") {
    auto gen = []() -> Generator<std::unique_ptr<int>> {
        co_yield std::make_unique<int>(1);
        co_yield std::make_unique<int>(2);
        co_yield std::make_unique<int>(3);
    }();

    auto it = gen.begin();
    std::unique_ptr<int> ptr1 = std::move(*it);
    REQUIRE(*ptr1 == 1);

    ++it;
    std::unique_ptr<int> ptr2 = std::move(*it);
    REQUIRE(*ptr2 == 2);

    ++it;
    std::unique_ptr<int> ptr3 = std::move(*it);
    REQUIRE(*ptr3 == 3);

    ++it;
    REQUIRE(it == std::default_sentinel);
}

TEST_CASE("Move generator with move-only content", "[Generator]") {
    auto gen1 = []() -> Generator<std::unique_ptr<int>> {
        co_yield std::make_unique<int>(42);
    }();

    auto gen2 = std::move(gen1);

    auto it = gen2.begin();
    REQUIRE(**it == 42);

    REQUIRE(gen1.begin() == std::default_sentinel);
}

TEST_CASE("Stress test: large sequence", "[Generator][stress]") {
    constexpr int N = 100'000;
    auto gen = generate_numbers(N);
    int counter = 0;

    for (int val : gen) {
        REQUIRE(val == counter++);
    }

    REQUIRE(counter == N);
}

TEST_CASE("Stress test: move between iterations", "[Generator][stress]") {
    constexpr int N = 10'000;
    auto gen1 = generate_numbers(N);
    auto it = gen1.begin();

    auto gen2 = std::move(gen1);
    int counter = 0;

    for (; it != std::default_sentinel; ++it) {
        REQUIRE(*it == counter++);
    }

    REQUIRE(counter == N);
}

TEST_CASE("Stress test: nested generators", "[Generator][stress]") {
    auto nested_gen = []() -> Generator<int> {
        for (int i : generate_numbers(1000)) {
            co_yield i * 2;
        }
    };

    int counter = 0;
    for (int val : nested_gen()) {
        REQUIRE(val == counter++ * 2);
    }
    REQUIRE(counter == 1000);
}

TEST_CASE("Stress test with move-only types", "[Generator][stress]") {
    constexpr int N = 10'000;

    auto gen = []() -> Generator<std::unique_ptr<int>> {
        for (int i = 0; i < N; ++i) {
            co_yield std::make_unique<int>(i);
        }
    }();

    int counter = 0;
    for (auto&& ptr : gen) {
        REQUIRE(*ptr == counter++);

        std::unique_ptr<int> stolen = std::move(ptr);
        REQUIRE(*stolen == counter - 1);
    }
    REQUIRE(counter == N);
}
