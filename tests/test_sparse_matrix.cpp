#include "fast/sparse_matrix.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SparseMatrix basic operations", "[SparseMatrix]") {
    using namespace topa::fast;

    SparseMatrix matrix;

    SECTION("Empty matrix behavior") {
        REQUIRE_FALSE(matrix.Contains(0));
        REQUIRE_FALSE(matrix.Contains(1));
    }

    SECTION("Column creation and access") {
        auto& row0 = matrix[0];
        row0.push_back(1.0);  // Предполагая, что Row поддерживает push_back

        REQUIRE(matrix.Contains(0));
        REQUIRE(matrix[0].size() == 1);
        REQUIRE_FALSE(matrix.Contains(1));

        // Проверяем независимость столбцов
        auto& row1 = matrix[1];
        row1.push_back(2.0);
        REQUIRE(matrix.Contains(1));
        REQUIRE(matrix[1].size() == 1);
    }

    SECTION("Column erasure") {
        matrix[5];
        REQUIRE(matrix.Contains(5));

        matrix.Erase(5);
        REQUIRE_FALSE(matrix.Contains(5));
    }

    SECTION("Reserve method") {
        REQUIRE_NOTHROW(matrix.Reserve(10));

        // Добавляем элементы после резервирования
        for (int i = 0; i < 10; i++) {
            matrix[i];
        }
        REQUIRE(matrix.Contains(9));
    }

    SECTION("Row modification persistence") {
        auto& row = matrix[3];
        row.push_back(3);

        const auto& same_row = matrix[3];
        REQUIRE(same_row.size() == 1);
        REQUIRE(same_row[0] == 3);
    }

    SECTION("Multiple operations sequence") {
        REQUIRE_FALSE(matrix.Contains(2));

        matrix[2];
        REQUIRE(matrix.Contains(2));

        matrix.Erase(2);
        REQUIRE_FALSE(matrix.Contains(2));

        matrix[2];
        matrix[2].push_back(10);
        REQUIRE(matrix.Contains(2));
        REQUIRE(matrix[2].size() == 1);
    }
}
