#include "reducer.hpp"
#include "boundary_matrix.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace topa;

TEST_CASE("Twist and DoubleTwist produce same matrix (tetrahedron)") {
    WSimplices wsimplices = {
        {{0}, 0.0f},       {{1}, 0.0f},       {{2}, 0.0f},
        {{3}, 0.0f},       {{0, 1}, 1.0f},    {{0, 2}, 1.0f},
        {{0, 3}, 1.0f},    {{1, 2}, 1.0f},    {{1, 3}, 1.0f},
        {{2, 3}, 1.0f},    {{0, 1, 2}, 2.0f}, {{0, 1, 3}, 2.0f},
        {{0, 2, 3}, 2.0f}, {{1, 2, 3}, 2.0f},
    };
    FilteredComplex complex(std::move(wsimplices), 4);

    Reducer twist = Reducer::Twist();
    Reducer double_twist = Reducer::DoubleTwist();

    Lows twist_result = twist.Reduce(complex);
    Lows double_twist_result = double_twist.Reduce(complex);
    REQUIRE(twist_result == double_twist_result);
}

TEST_CASE("Twist and DoubleTwist produce same matrix (5 points)") {
    WSimplices wsimplices;

    for (VertexId i = 0; i < 5; ++i) {
        wsimplices.push_back({{i}, 0.0f});
    }

    const std::vector<std::pair<VertexId, VertexId>> edges = {
        {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 2},
        {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}};
    for (const auto& [u, v] : edges) {
        wsimplices.push_back({{u, v}, 1.0f});
    }

    const std::vector<std::tuple<VertexId, VertexId, VertexId>> triangles = {
        {0, 1, 2}, {0, 1, 3}, {0, 1, 4}, {0, 2, 3}, {0, 2, 4},
        {0, 3, 4}, {1, 2, 3}, {1, 2, 4}, {1, 3, 4}, {2, 3, 4}};
    for (const auto& [u, v, w] : triangles) {
        wsimplices.push_back({{u, v, w}, 2.0f});
    }
    std::sort(wsimplices.begin(), wsimplices.end());
    FilteredComplex complex(std::move(wsimplices), 5);

    Reducer twist = Reducer::Twist();
    Reducer double_twist = Reducer::DoubleTwist();

    Lows twist_result = twist.Reduce(complex);
    Lows double_twist_result = double_twist.Reduce(complex);
    REQUIRE(twist_result == double_twist_result);
}

TEST_CASE("Twist and DoubleTwist produce same matrix (full-VR)") {
    Pointcloud cloud = *Pointcloud::Load(DATA_DIR "/pointclouds/325.off");
    Filtration filtration = Filtration::FullVietorisRips();
    FilteredComplex complex = FilteredComplex::From(cloud, filtration);

    Reducer twist = Reducer::Twist();
    Reducer double_twist = Reducer::DoubleTwist();

    Lows twist_result = twist.Reduce(complex);
    Lows double_twist_result = double_twist.Reduce(complex);
    REQUIRE(twist_result == double_twist_result);
}
