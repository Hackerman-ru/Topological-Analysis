#include "common/boundary_matrix.hpp"
#include "common/coboundary_matrix.hpp"
#include "common/filtered_boundary_matrix.hpp"

#include "models/matrix.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <map>
#include <vector>
#include <cstddef>

// Моки для тестирования

using namespace topa;

namespace {

// Мок реализации Matrix
class TestMatrix : public models::Matrix<TestMatrix> {
   public:
    std::map<Position, Row> rows;

    const Row& operator[](std::size_t col) const {
        static Row empty;
        auto it = rows.find(col);
        return it != rows.end() ? it->second : empty;
    }

    bool Contains(std::size_t col) const {
        return rows.count(col) > 0;
    }

    std::size_t Size() const {
        return rows.size();
    }

    void Insert(std::size_t col, PositionRange auto&& row) {
        rows.try_emplace(col, std::make_move_iterator(row.begin()),
                         std::make_move_iterator(row.end()));
    }

    void Insert(std::size_t col, std::initializer_list<Position> row) {
        Insert(col, std::views::all(row));
    }

    Row& operator[](std::size_t col) {
        return rows[col];
    }

    void Reserve(std::size_t) {
        // Просто игнорируем для мока
    }

    void Erase(std::size_t col) {
        rows.erase(col);
    }
};

// Мок реализации FilteredComplex
class TestComplex : public models::FilteredComplex<TestComplex> {
   public:
    std::size_t SizeValue = 0;
    std::map<Position, std::vector<Position>> Facets;
    std::map<Position, std::vector<Position>> Cofacets;

    std::size_t Size() const {
        return SizeValue;
    }

    std::vector<Position> GetFacetsPosition(Position pos) const {
        auto it = Facets.find(pos);
        return it != Facets.end() ? it->second : std::vector<Position>{};
    }

    std::vector<Position> GetCofacetsPosition(Position pos) const {
        auto it = Cofacets.find(pos);
        return it != Cofacets.end() ? it->second : std::vector<Position>{};
    }
};

}  // namespace

TEST_CASE("BoundaryMatrix constructs matrix correctly") {
    using namespace topa::common;
    using namespace topa::models;

    TestComplex complex;
    complex.SizeValue = 3;
    complex.Facets = {{0, {}}, {1, {0, 2}}, {2, {1}}};

    auto matrix = BoundaryMatrix<TestMatrix, TestComplex>(complex);
    REQUIRE(matrix.Contains(1));
    REQUIRE(matrix.Contains(2));
    REQUIRE_FALSE(matrix.Contains(0));

    CHECK_THAT(matrix[1],
               Catch::Matchers::UnorderedEquals(std::vector<Position>{0, 2}));
    CHECK_THAT(matrix[2], Catch::Matchers::Equals(std::vector<Position>{1}));
}

TEST_CASE("BoundaryMatrixFiltered processes only given positions") {
    using namespace topa::common;
    using namespace topa::models;

    TestComplex complex;
    complex.SizeValue = 3;
    complex.Facets = {{0, {5}}, {1, {0, 2}}, {2, {1}}};

    std::vector<Position> positions = {1, 2};
    auto matrix =
        BoundaryMatrixFiltered<TestMatrix, TestComplex>(complex, positions);

    REQUIRE(matrix.Size() == 2);
    CHECK_THAT(matrix[1],
               Catch::Matchers::UnorderedEquals(std::vector<Position>{0, 2}));
    CHECK_THAT(matrix[2], Catch::Matchers::Equals(std::vector<Position>{1}));
    REQUIRE_FALSE(matrix.Contains(0));
}

TEST_CASE("CoboundaryMatrix inverts positions correctly") {
    using namespace topa::common;
    using namespace topa::models;

    TestComplex complex;
    complex.SizeValue = 3;
    complex.Cofacets = {{0, {1, 2}}, {1, {2}}, {2, {}}};

    auto matrix = CoboundaryMatrix<TestMatrix, TestComplex>(complex);

    REQUIRE(matrix.Contains(2));
    REQUIRE(matrix.Contains(1));

    CHECK_THAT(matrix[2],
               Catch::Matchers::UnorderedEquals(std::vector<Position>{0, 1}));
    CHECK_THAT(matrix[1], Catch::Matchers::Equals(std::vector<Position>{0}));
}
