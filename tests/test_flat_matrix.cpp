#include "flat_matrix.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <vector>

using namespace topa::detail;

TEST_CASE("FlatMatrix constructor and basic properties", "[FlatMatrix]") {
    FlatMatrix<int> matrix(3);
    REQUIRE(matrix.size() == 0);

    matrix.push_back({1, 2, 3});
    REQUIRE(matrix.size() == 1);
    REQUIRE(matrix[0][0] == 1);
    REQUIRE(matrix[0][1] == 2);
    REQUIRE(matrix[0][2] == 3);
}

TEST_CASE("push_back and pop_back", "[FlatMatrix]") {
    FlatMatrix<int> matrix(2);
    matrix.push_back({1, 2});
    REQUIRE(matrix.size() == 1);

    matrix.push_back(std::vector<int> {3, 4});
    REQUIRE(matrix.size() == 2);

    matrix.pop_back();
    REQUIRE(matrix.size() == 1);
    REQUIRE(matrix[0][0] == 1);
}

TEST_CASE("Element access via Row and CRow", "[FlatMatrix]") {
    FlatMatrix<int> matrix(2);
    matrix.push_back({1, 2});
    matrix.push_back({3, 4});

    auto row0 = matrix[0];
    REQUIRE(row0[0] == 1);
    REQUIRE(row0[1] == 2);

    row0[1] = 5;
    REQUIRE(matrix[0][1] == 5);

    const auto& cmatrix = matrix;
    auto crow1 = cmatrix[1];
    REQUIRE(crow1[0] == 3);
    REQUIRE(crow1[1] == 4);
}

TEST_CASE("front and back", "[FlatMatrix]") {
    FlatMatrix<int> matrix(2);
    matrix.push_back({1, 2});
    matrix.push_back({3, 4});

    REQUIRE(matrix.front()[0] == 1);
    REQUIRE(matrix.back()[1] == 4);

    matrix.pop_back();
    REQUIRE(matrix.back()[0] == 1);
}

TEST_CASE("Iterators", "[FlatMatrix]") {
    FlatMatrix<int> matrix(2);
    matrix.push_back({1, 2});
    matrix.push_back({3, 4});

    std::vector<int> all_elements(matrix.begin(), matrix.end());
    REQUIRE(all_elements == std::vector<int> {1, 2, 3, 4});

    auto row = matrix[1];
    std::vector<int> row_elements(row.begin(), row.end());
    REQUIRE(row_elements == std::vector<int> {3, 4});
}

TEST_CASE("Conversion to vector", "[FlatMatrix]") {
    FlatMatrix<int> matrix(3);
    matrix.push_back({1, 2, 3});

    std::vector<int> row0_vec = matrix[0];
    REQUIRE(row0_vec == std::vector<int> {1, 2, 3});
}

TEST_CASE("Clear and reserve", "[FlatMatrix]") {
    FlatMatrix<int> matrix(2);
    matrix.reserve(10);
    matrix.push_back({1, 2});
    matrix.clear();
    REQUIRE(matrix.size() == 0);
}

TEST_CASE("Const FlatMatrix access", "[FlatMatrix]") {
    FlatMatrix<int> matrix(2);
    matrix.push_back({1, 2});
    const auto& cmatrix = matrix;

    auto crow = cmatrix[0];
    REQUIRE(crow[1] == 2);

    std::vector<int> vec = crow;
    REQUIRE(vec == std::vector<int> {1, 2});
}

TEST_CASE("Row iterators", "[FlatMatrix]") {
    FlatMatrix<int> matrix(3);
    matrix.push_back({1, 2, 3});
    auto row = matrix[0];
    auto it = row.begin();

    REQUIRE(*it == 1);
    ++it;
    REQUIRE(*it == 2);
    it += 2;
    REQUIRE(it == row.end());
}

TEST_CASE("Stress test: many rows", "[FlatMatrix][stress]") {
    constexpr size_t ROW_SIZE = 5;
    constexpr size_t NUM_ROWS = 100000;

    FlatMatrix<int> matrix(ROW_SIZE);
    matrix.reserve(NUM_ROWS);

    for (size_t i = 0; i < NUM_ROWS; ++i) {
        std::vector<int> row(ROW_SIZE, static_cast<int>(i));
        matrix.push_back(row);
    }

    REQUIRE(matrix.size() == NUM_ROWS);
    REQUIRE(matrix[0][0] == 0);
    REQUIRE(matrix[999][0] == 999);
    REQUIRE(matrix[NUM_ROWS - 1][ROW_SIZE - 1] == static_cast<int>(NUM_ROWS - 1));
}

TEST_CASE("Stress test: large data", "[FlatMatrix][stress]") {
    constexpr size_t ROW_SIZE = 1000;
    constexpr size_t NUM_ROWS = 1000;

    FlatMatrix<std::vector<int>> matrix(ROW_SIZE);
    for (size_t i = 0; i < NUM_ROWS; ++i) {
        std::vector<std::vector<int>> row;
        row.reserve(ROW_SIZE);
        for (size_t j = 0; j < ROW_SIZE; ++j) {
            row.emplace_back(1000, static_cast<int>(i * j));
        }
        matrix.push_back(std::move(row));
    }

    REQUIRE(matrix.size() == NUM_ROWS);
    REQUIRE(matrix[0][0].size() == 1000);
    REQUIRE(matrix[0][0][0] == 0);
}
