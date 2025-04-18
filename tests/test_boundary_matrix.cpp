#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include "boundary_matrix.hpp"
#include "filtered_complex.hpp"
#include "simplex.hpp"
#include "reducer.hpp"
#include <iostream>

using namespace topa;

TEST_CASE(
    "BoundaryMatrix returns correct facets for vertices, edges, and triangle",
    "[boundary]") {
    WSimplices wsimplices;

    wsimplices.push_back(WSimplex{Simplex{0}, 0.0});
    wsimplices.push_back(WSimplex{Simplex{1}, 0.0});
    wsimplices.push_back(WSimplex{Simplex{2}, 0.0});

    wsimplices.push_back(WSimplex{Simplex{0, 1}, 1.0});
    wsimplices.push_back(WSimplex{Simplex{1, 2}, 1.0});
    wsimplices.push_back(WSimplex{Simplex{0, 2}, 1.0});

    wsimplices.push_back(WSimplex{Simplex{0, 1, 2}, 2.0});

    FilteredComplex complex(std::move(wsimplices), 3);
    Matrix boundary = BoundaryMatrix(complex);

    for (size_t i = 0; i < 3; ++i) {
        REQUIRE(boundary[i].empty());
    }

    REQUIRE_THAT(boundary[3],
                 Catch::Matchers::UnorderedEquals(Positions{0, 1}));
    REQUIRE_THAT(boundary[4],
                 Catch::Matchers::UnorderedEquals(Positions{1, 2}));
    REQUIRE_THAT(boundary[5],
                 Catch::Matchers::UnorderedEquals(Positions{0, 2}));

    REQUIRE_THAT(boundary[6],
                 Catch::Matchers::UnorderedEquals(Positions{3, 4, 5}));
}

TEST_CASE("CoboundaryMatrix returns correct cofacets for edges and triangle",
          "[coboundary]") {
    WSimplices wsimplices;

    wsimplices.push_back(WSimplex{Simplex{0}, 0.0});
    wsimplices.push_back(WSimplex{Simplex{1}, 0.0});
    wsimplices.push_back(WSimplex{Simplex{2}, 0.0});
    wsimplices.push_back(WSimplex{Simplex{0, 1}, 1.0});
    wsimplices.push_back(WSimplex{Simplex{1, 2}, 1.0});
    wsimplices.push_back(WSimplex{Simplex{0, 2}, 1.0});
    wsimplices.push_back(WSimplex{Simplex{0, 1, 2}, 2.0});

    FilteredComplex complex(std::move(wsimplices), 3);
    Matrix coboundary = CoboundaryMatrix(complex);

    REQUIRE_THAT(coboundary[3], Catch::Matchers::UnorderedEquals(Positions{6}));
    REQUIRE_THAT(coboundary[4], Catch::Matchers::UnorderedEquals(Positions{6}));
    REQUIRE_THAT(coboundary[5], Catch::Matchers::UnorderedEquals(Positions{6}));

    REQUIRE(coboundary[6].empty());
}

TEST_CASE("BoundaryMatrixFiltered includes only specified positions",
          "[boundary]") {
    WSimplices wsimplices;

    wsimplices.push_back(WSimplex{Simplex{0}, 0.0});
    wsimplices.push_back(WSimplex{Simplex{1}, 0.0});
    wsimplices.push_back(WSimplex{Simplex{2}, 0.0});
    wsimplices.push_back(WSimplex{Simplex{0, 1}, 1.0});
    wsimplices.push_back(WSimplex{Simplex{1, 2}, 1.0});
    wsimplices.push_back(WSimplex{Simplex{0, 2}, 1.0});
    wsimplices.push_back(WSimplex{Simplex{0, 1, 2}, 2.0});

    FilteredComplex complex(std::move(wsimplices), 3);
    Positions positions = {3, 6};
    Matrix filtered = BoundaryMatrixFiltered(complex, positions);

    REQUIRE(filtered.size() == 2);

    REQUIRE_THAT(filtered[3],
                 Catch::Matchers::UnorderedEquals(Positions{0, 1}));
    REQUIRE_THAT(filtered[6],
                 Catch::Matchers::UnorderedEquals(Positions{3, 4, 5}));

    for (size_t pos : {0, 1, 2, 4, 5}) {
        REQUIRE(filtered[pos].empty());
    }
}

TEST_CASE("BoundaryMatrix handles empty complex", "[boundary]") {
    WSimplices wsimplices;
    FilteredComplex complex(std::move(wsimplices), 0);
    Matrix boundary = BoundaryMatrix(complex);
    REQUIRE(boundary.empty());
}

TEST_CASE("BoundaryMatrix for 3-simplex (tetrahedron)",
          "[boundary][advanced]") {
    WSimplices wsimplices;

    wsimplices.push_back({{0}, 0.0f});
    wsimplices.push_back({{1}, 0.0f});
    wsimplices.push_back({{2}, 0.0f});
    wsimplices.push_back({{3}, 0.0f});

    wsimplices.push_back({{0, 1}, 1.0f});
    wsimplices.push_back({{0, 2}, 1.0f});
    wsimplices.push_back({{0, 3}, 1.0f});
    wsimplices.push_back({{1, 2}, 1.0f});
    wsimplices.push_back({{1, 3}, 1.0f});
    wsimplices.push_back({{2, 3}, 1.0f});

    wsimplices.push_back({{0, 1, 2}, 2.0f});
    wsimplices.push_back({{0, 1, 3}, 2.0f});
    wsimplices.push_back({{0, 2, 3}, 2.0f});
    wsimplices.push_back({{1, 2, 3}, 2.0f});

    FilteredComplex complex(std::move(wsimplices), 4);
    Matrix boundary = BoundaryMatrix(complex);

    REQUIRE(boundary.size() == 14);
    REQUIRE_THAT(boundary[4],
                 Catch::Matchers::UnorderedEquals(Positions{0, 1}));
    REQUIRE_THAT(boundary[10],
                 Catch::Matchers::UnorderedEquals(Positions{4, 5, 7}));
    REQUIRE_THAT(boundary[11],
                 Catch::Matchers::UnorderedEquals(Positions{4, 6, 8}));
    REQUIRE_THAT(boundary[13],
                 Catch::Matchers::UnorderedEquals(Positions{7, 8, 9}));
}

TEST_CASE("CoboundaryMatrix for vertex in complex", "[coboundary][advanced]") {
    WSimplices wsimplices;

    wsimplices.push_back({{0}, 0.0f});
    wsimplices.push_back({{1}, 0.0f});
    wsimplices.push_back({{2}, 0.0f});
    wsimplices.push_back({{0, 1}, 1.0f});
    wsimplices.push_back({{0, 2}, 1.0f});
    wsimplices.push_back({{1, 2}, 1.0f});

    FilteredComplex complex(std::move(wsimplices), 3);
    Matrix coboundary = CoboundaryMatrix(complex);

    REQUIRE_THAT(coboundary[0],
                 Catch::Matchers::UnorderedEquals(Positions{3, 4}));
}

TEST_CASE("BoundaryMatrixFiltered with reversed positions",
          "[boundary][filtered]") {
    WSimplices wsimplices;

    wsimplices.push_back({{0}, 0.0f});
    wsimplices.push_back({{1}, 0.0f});
    wsimplices.push_back({{0, 1}, 1.0f});

    FilteredComplex complex(std::move(wsimplices), 2);
    Positions positions = {2, 1, 0};
    Matrix filtered = BoundaryMatrixFiltered(complex, positions);

    REQUIRE(filtered.size() == 3);
    REQUIRE_THAT(filtered[2],
                 Catch::Matchers::UnorderedEquals(Positions{0, 1}));
    REQUIRE(filtered[1].empty());
    REQUIRE(filtered[0].empty());
}

TEST_CASE("BoundaryMatrixFiltered with empty positions", "[boundary][edge]") {
    WSimplices wsimplices = {{{0}, 0.0f}, {{1}, 0.0f}, {{0, 1}, 1.0f}};
    FilteredComplex complex(std::move(wsimplices), 2);

    Positions positions;
    Matrix filtered = BoundaryMatrixFiltered(complex, positions);

    REQUIRE(filtered.empty());
}

TEST_CASE("BoundaryMatrix with multiple cofacets", "[boundary][cofacets]") {
    WSimplices wsimplices = {
        {{0}, 0.0f},    {{1}, 0.0f},    {{2}, 0.0f},       {{0, 1}, 1.0f},
        {{0, 2}, 1.0f}, {{1, 2}, 1.0f}, {{0, 1, 2}, 2.0f}, {{0, 1, 3}, 2.0f}};

    FilteredComplex complex(std::move(wsimplices), 4);
    Matrix coboundary = CoboundaryMatrix(complex);

    REQUIRE_THAT(coboundary[3],
                 Catch::Matchers::UnorderedEquals(Positions{6, 7}));
}

TEST_CASE("Matrix structure validation", "[boundary][matrix]") {
    WSimplices wsimplices = {{{0}, 0.0f}, {{1}, 0.0f}, {{0, 1}, 1.0f}};
    FilteredComplex complex(std::move(wsimplices), 2);

    Matrix boundary = BoundaryMatrix(complex);

    REQUIRE(boundary.size() == 3);
    REQUIRE(boundary.contains(0));
    REQUIRE(boundary.contains(1));
    REQUIRE(boundary.contains(2));

    REQUIRE_THAT(boundary.at(2),
                 Catch::Matchers::UnorderedEquals(Positions{0, 1}));
}

TEST_CASE("BoundaryMatrix for full 2-skeleton (triangle complex)",
          "[boundary][2-skeleton]") {
    WSimplices wsimplices;

    // Вершины (0, 1, 2)
    wsimplices.push_back({{0}, 0.0f});
    wsimplices.push_back({{1}, 0.0f});
    wsimplices.push_back({{2}, 0.0f});

    // Все рёбра (0-1, 0-2, 1-2)
    wsimplices.push_back({{0, 1}, 1.0f});
    wsimplices.push_back({{0, 2}, 1.0f});
    wsimplices.push_back({{1, 2}, 1.0f});

    // Треугольник (0-1-2)
    wsimplices.push_back({{0, 1, 2}, 2.0f});

    // Убедимся, что комплекс упорядочен
    std::sort(wsimplices.begin(), wsimplices.end());

    FilteredComplex complex(std::move(wsimplices), 3);
    Matrix boundary = BoundaryMatrix(complex);
    Matrix coboundary = CoboundaryMatrix(complex);

    SECTION("Check boundary for edges") {
        REQUIRE_THAT(boundary[3], Catch::Matchers::UnorderedEquals(
                                      Positions{0, 1}));  // Edge 0-1
        REQUIRE_THAT(boundary[4], Catch::Matchers::UnorderedEquals(
                                      Positions{0, 2}));  // Edge 0-2
        REQUIRE_THAT(boundary[5], Catch::Matchers::UnorderedEquals(
                                      Positions{1, 2}));  // Edge 1-2
    }

    SECTION("Check boundary for triangle") {
        REQUIRE_THAT(boundary[6], Catch::Matchers::UnorderedEquals(
                                      Positions{3, 4, 5}));  // Triangle
    }

    SECTION("Check coboundary for vertices") {
        REQUIRE_THAT(coboundary[0], Catch::Matchers::UnorderedEquals(
                                        Positions{3, 4}));  // Vertex 0
        REQUIRE_THAT(coboundary[1], Catch::Matchers::UnorderedEquals(
                                        Positions{3, 5}));  // Vertex 1
        REQUIRE_THAT(coboundary[2], Catch::Matchers::UnorderedEquals(
                                        Positions{4, 5}));  // Vertex 2
    }

    SECTION("Check coboundary for edges") {
        REQUIRE_THAT(coboundary[3], Catch::Matchers::UnorderedEquals(
                                        Positions{6}));  // Edge 0-1
        REQUIRE_THAT(coboundary[4], Catch::Matchers::UnorderedEquals(
                                        Positions{6}));  // Edge 0-2
        REQUIRE_THAT(coboundary[5], Catch::Matchers::UnorderedEquals(
                                        Positions{6}));  // Edge 1-2
    }

    SECTION("Check coboundary for triangle") {
        REQUIRE(coboundary[6].empty());  // Triangle has no cofacets
    }
}

// Тест для полного 2-скелета с четырьмя вершинами (4 треугольника)
TEST_CASE("BoundaryMatrix for full 2-skeleton (4 vertices)",
          "[boundary][2-skeleton]") {
    WSimplices wsimplices;

    // Вершины (0, 1, 2, 3)
    for (VertexId i = 0; i < 4; ++i) {
        wsimplices.push_back({{i}, 0.0f});
    }

    // Все рёбра (C(4,2) = 6)
    wsimplices.push_back({{0, 1}, 1.0f});
    wsimplices.push_back({{0, 2}, 1.0f});
    wsimplices.push_back({{0, 3}, 1.0f});
    wsimplices.push_back({{1, 2}, 1.0f});
    wsimplices.push_back({{1, 3}, 1.0f});
    wsimplices.push_back({{2, 3}, 1.0f});

    // Все треугольники (C(4,3) = 4)
    wsimplices.push_back({{0, 1, 2}, 2.0f});
    wsimplices.push_back({{0, 1, 3}, 2.0f});
    wsimplices.push_back({{0, 2, 3}, 2.0f});
    wsimplices.push_back({{1, 2, 3}, 2.0f});

    // Упорядочим по весу и размерности
    std::sort(wsimplices.begin(), wsimplices.end());

    FilteredComplex complex(std::move(wsimplices), 4);
    Matrix boundary = BoundaryMatrix(complex);
    Matrix coboundary = CoboundaryMatrix(complex);

    SECTION("Check boundaries of triangles") {
        REQUIRE_THAT(boundary[10], Catch::Matchers::UnorderedEquals(
                                       Positions{4, 5, 7}));  // Triangle 0-1-2
        REQUIRE_THAT(boundary[11], Catch::Matchers::UnorderedEquals(
                                       Positions{4, 6, 8}));  // Triangle 0-1-3
        REQUIRE_THAT(boundary[12], Catch::Matchers::UnorderedEquals(
                                       Positions{5, 6, 9}));  // Triangle 0-2-3
        REQUIRE_THAT(boundary[13], Catch::Matchers::UnorderedEquals(
                                       Positions{7, 8, 9}));  // Triangle 1-2-3
    }

    SECTION("Check coboundaries of edges") {
        REQUIRE_THAT(coboundary[4], Catch::Matchers::UnorderedEquals(
                                        Positions{10, 11}));  // Edge 0-1
        REQUIRE_THAT(coboundary[5], Catch::Matchers::UnorderedEquals(
                                        Positions{10, 12}));  // Edge 0-2
        REQUIRE_THAT(coboundary[6], Catch::Matchers::UnorderedEquals(
                                        Positions{11, 12}));  // Edge 0-3
        REQUIRE_THAT(coboundary[7], Catch::Matchers::UnorderedEquals(
                                        Positions{10, 13}));  // Edge 1-2
        REQUIRE_THAT(coboundary[8], Catch::Matchers::UnorderedEquals(
                                        Positions{11, 13}));  // Edge 1-3
        REQUIRE_THAT(coboundary[9], Catch::Matchers::UnorderedEquals(
                                        Positions{12, 13}));  // Edge 2-3
    }
}

void PrintMatrix(const Matrix& matrix, size_t n, const std::string& title) {
    std::cout << "\n" << title << " (size = " << n << "):\n";

    // Заголовок с номерами столбцов
    std::cout << "     ";
    for (size_t col = 0; col < n; ++col) {
        std::cout << std::setw(3) << col << " ";
    }
    std::cout << "\n";

    // Тело матрицы
    for (size_t row = 0; row < n; ++row) {
        std::cout << std::setw(3) << row << "|";
        for (size_t col = 0; col < n; ++col) {
            if (matrix.find(col) != matrix.end()) {
                const auto& col_data = matrix.at(col);
                bool has_row = std::find(col_data.begin(), col_data.end(),
                                         row) != col_data.end();
                std::cout << (has_row ? "  * " : "  . ");
            } else {
                std::cout << "  . ";
            }
        }
        std::cout << "\n";
    }
}

TEST_CASE("Print Boundary and Coboundary matrices", "[visual][boundary]") {
    WSimplices wsimplices = {
        {{0}, 0.0f},    {{1}, 0.0f},    {{2}, 0.0f},    {{3}, 0.0f},
        {{0, 2}, 1.0f}, {{1, 2}, 2.0f}, {{1, 3}, 3.0f}, {{0, 3}, 4.0f},
        {{2, 3}, 5.0f}, {{0, 1}, 6.0f},
    };
    std::sort(wsimplices.begin(), wsimplices.end());

    FilteredComplex complex(std::move(wsimplices), 4);
    const size_t n = complex.GetSimplices().size();

    // Выводим информацию о симплексах
    std::cout << "Simplices order:\n";
    for (size_t i = 0; i < n; ++i) {
        const auto& s = complex.GetSimplices()[i];
        std::cout << "pos " << i << ": ";
        for (auto v : s.simplex) std::cout << v << " ";
        std::cout << "(dim=" << s.simplex.size() - 1 << ", weight=" << s.weight
                  << ")\n";
    }

    // Граничная матрица
    Matrix boundary = BoundaryMatrix(complex);
    PrintMatrix(boundary, n, "Boundary Matrix");

    // Кограничная матрица
    Matrix coboundary = CoboundaryMatrix(complex);
    PrintMatrix(coboundary, n, "Coboundary Matrix");
}
