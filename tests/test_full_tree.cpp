#include "fast/full_tree.hpp"

#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <random>
#include <ranges>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <iostream>

using namespace topa;
using namespace topa::fast;
using Positions = std::vector<Position>;

void FillFull2Skeleton(FullTree& tree, size_t n) {
    for (VertexId v = 0; v < n; ++v) {
        tree.Add({v}, v);
    }

    for (VertexId u = 0; u < n; ++u) {
        for (VertexId v = u + 1; v < n; ++v) {
            tree.Add({u, v}, u * 100 + v);
        }
    }

    for (VertexId u = 0; u < n; ++u) {
        for (VertexId v = u + 1; v < n; ++v) {
            for (VertexId w = v + 1; w < n; ++w) {
                tree.Add({u, v, w}, u * 10000 + v * 100 + w);
            }
        }
    }
}

TEST_CASE("FullTree assumes full 2-skeleton is pre-added") {
    const size_t n = 3;
    FullTree tree(n);
    FillFull2Skeleton(tree, n);

    SECTION("All vertices exist") {
        REQUIRE(tree.GetPos({0}) == 0);
        REQUIRE(tree.GetPos({1}) == 1);
        REQUIRE(tree.GetPos({2}) == 2);
    }

    SECTION("All edges exist") {
        REQUIRE(tree.GetPos({0, 1}) == 0 * 100 + 1);
        REQUIRE(tree.GetPos({0, 2}) == 0 * 100 + 2);
        REQUIRE(tree.GetPos({1, 2}) == 1 * 100 + 2);
    }

    SECTION("All triangles exist") {
        REQUIRE(tree.GetPos({0, 1, 2}) == 0 * 10000 + 1 * 100 + 2);
    }

    SECTION("Has() returns true for all simplices up to dim 2") {
        REQUIRE(tree.Has({0}));
        REQUIRE(tree.Has({0, 1}));
        REQUIRE(tree.Has({0, 1, 2}));
        REQUIRE_FALSE(tree.Has({0, 1, 2, 3}));  // Симплекс высокой размерности
    }
}

TEST_CASE("GetFacetsPos returns all facets from full 2-skeleton") {
    const size_t n = 4;
    FullTree tree(n);
    FillFull2Skeleton(tree, n);

    SECTION("Facets of a triangle") {
        auto facets = tree.GetFacetsPos({1, 2, 3});
        REQUIRE(facets.size() == 3);
        // Позиции рёбер (1,2), (1,3), (2,3)
        REQUIRE(std::find(facets.begin(), facets.end(), 1 * 100 + 2) !=
                facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 1 * 100 + 3) !=
                facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 2 * 100 + 3) !=
                facets.end());
    }

    SECTION("Facets of an edge") {
        auto facets = tree.GetFacetsPos({0, 3});
        REQUIRE(facets == Positions{0, 3});  // Позиции вершин 0 и 3
    }
}

TEST_CASE("GetCofacetsPos returns all cofacets from full 2-skeleton") {
    const size_t n = 4;
    FullTree tree(n);
    FillFull2Skeleton(tree, n);

    SECTION("Cofacets of an edge (0,1)") {
        auto cofacets = tree.GetCofacetsPos({0, 1});
        // Все треугольники, содержащие ребро (0,1): (0,1,2), (0,1,3)
        REQUIRE(cofacets.size() == 2);
        REQUIRE(std::find(cofacets.begin(), cofacets.end(),
                          0 * 10000 + 1 * 100 + 2) != cofacets.end());
        REQUIRE(std::find(cofacets.begin(), cofacets.end(),
                          0 * 10000 + 1 * 100 + 3) != cofacets.end());
    }

    SECTION("Cofacets of a vertex (1)") {
        auto cofacets = tree.GetCofacetsPos({1});
        // Все рёбра, содержащие вершину 1: (0,1), (1,2), (1,3)
        REQUIRE(cofacets.size() == 3);
        REQUIRE(std::find(cofacets.begin(), cofacets.end(), 0 * 100 + 1) !=
                cofacets.end());
        REQUIRE(std::find(cofacets.begin(), cofacets.end(), 1 * 100 + 2) !=
                cofacets.end());
        REQUIRE(std::find(cofacets.begin(), cofacets.end(), 1 * 100 + 3) !=
                cofacets.end());
    }
}

TEST_CASE("Edge and triangle index calculation") {
    SECTION("Edge indices for n=4") {
        FullTree tree(4);
        FillFull2Skeleton(tree, 4);

        // Проверка индексов рёбер
        REQUIRE(tree.GetPos({0, 1}) == 0 * 100 + 1);  // Индекс 0
        REQUIRE(tree.GetPos({0, 2}) == 0 * 100 + 2);  // Индекс 1
        REQUIRE(tree.GetPos({0, 3}) == 0 * 100 + 3);  // Индекс 2
        REQUIRE(tree.GetPos({1, 2}) == 1 * 100 + 2);  // Индекс 3
        REQUIRE(tree.GetPos({1, 3}) == 1 * 100 + 3);  // Индекс 4
        REQUIRE(tree.GetPos({2, 3}) == 2 * 100 + 3);  // Индекс 5
    }

    SECTION("Triangle indices for n=4") {
        FullTree tree(4);
        FillFull2Skeleton(tree, 4);

        // Проверка индексов треугольников
        REQUIRE(tree.GetPos({0, 1, 2}) == 0 * 10000 + 1 * 100 + 2);  // Индекс 0
        REQUIRE(tree.GetPos({0, 1, 3}) == 0 * 10000 + 1 * 100 + 3);  // Индекс 1
        REQUIRE(tree.GetPos({0, 2, 3}) == 0 * 10000 + 2 * 100 + 3);  // Индекс 2
        REQUIRE(tree.GetPos({1, 2, 3}) == 1 * 10000 + 2 * 100 + 3);  // Индекс 3
    }
}

TEST_CASE("Stress test with n=1000 vertices") {
    constexpr size_t n = 500;
    FullTree tree(n);
    FillFull2Skeleton(tree, n);

    SECTION("Verify edge positions") {
        const VertexId u = 123, v = 456;  // u < v
        const Position expected_pos = u * 100 + v;
        REQUIRE(tree.GetPos({u, v}) == expected_pos);
    }

    SECTION("Verify triangle cofacets for random edge") {
        const VertexId u = 100, v = 200;
        auto cofacets = tree.GetCofacetsPos({u, v});

        REQUIRE(cofacets.size() == n - 2);

        // Проверяем несколько случайных треугольников
        REQUIRE(tree.GetPos({u, v, v + 1}) == u * 10000 + v * 100 + (v + 1));
        REQUIRE(tree.GetPos({u, v, n - 1}) == u * 10000 + v * 100 + (n - 1));
    }
}

TEST_CASE("Randomized property-based testing") {
    constexpr size_t n = 50;
    FullTree tree(n);
    FillFull2Skeleton(tree, n);

    std::mt19937 gen(42);
    std::uniform_int_distribution<VertexId> vertex_dist(0, n - 1);

    SECTION("Validate random triangles with unique vertices") {
        for (int i = 0; i < 1000; ++i) {
            VertexId u, v, w;
            do {
                u = vertex_dist(gen);
                v = vertex_dist(gen);
                w = vertex_dist(gen);

                if (u > v)
                    std::swap(u, v);
                if (v > w)
                    std::swap(v, w);
                if (u > v)
                    std::swap(u, v);
            } while (u == v || v == w || u == w);

            // Проверяем расчёт позиции
            const Position expected_pos = u * 10000 + v * 100 + w;
            REQUIRE(tree.GetPos({u, v, w}) == expected_pos);

            // Проверяем грани
            auto facets = tree.GetFacetsPos({u, v, w});
            REQUIRE(facets.size() == 3);

            const Position uv_pos = u * 100 + v;
            const Position uw_pos = u * 100 + w;
            const Position vw_pos = v * 100 + w;

            REQUIRE_THAT(facets, Catch::Matchers::Contains(uv_pos));
            REQUIRE_THAT(facets, Catch::Matchers::Contains(uw_pos));
            REQUIRE_THAT(facets, Catch::Matchers::Contains(vw_pos));
        }
    }

    SECTION("Ensure all possible triangles are accessible") {
        constexpr size_t expected_triangles = 50 * 49 * 48 / 6;
        size_t counted_triangles = 0;

        for (VertexId u = 0; u < n; ++u) {
            for (VertexId v = u + 1; v < n; ++v) {
                for (VertexId w = v + 1; w < n; ++w) {
                    REQUIRE(tree.GetPos({u, v, w}) == u * 10000 + v * 100 + w);
                    counted_triangles++;
                }
            }
        }

        REQUIRE(counted_triangles == expected_triangles);
    }
}
