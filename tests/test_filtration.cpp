#include "filtration.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <algorithm>
#include <vector>

using namespace topa;
using namespace Catch::Matchers;

TEST_CASE("Filtration with empty cloud") {
    Pointcloud cloud(2);
    auto filtration = Filtration::VietorisRips(1.0, 2);
    auto generator = filtration.Build(cloud);
    CHECK(std::distance(generator.begin(), generator.end()) == 0);
}

TEST_CASE("Single point cloud") {
    Pointcloud cloud(2);
    cloud.Add({0, 0});
    auto filtration = Filtration::VietorisRips(1.0, 2);
    auto generator = filtration.Build(cloud);
    std::vector<WSimplex> simplices(generator.begin(), generator.end());

    CHECK(simplices.size() == 1);
    CHECK(simplices[0].simplex == Simplex{0});
    CHECK(simplices[0].weight == 0.0f);
}

TEST_CASE("Two points within radius") {
    Pointcloud cloud(2);
    cloud.Add({0, 0});
    cloud.Add({1, 0});
    auto filtration = Filtration::VietorisRips(1.0, 1);
    auto generator = filtration.Build(cloud);
    std::vector<WSimplex> simplices(generator.begin(), generator.end());

    CHECK(simplices.size() == 3);
    CHECK_THAT(simplices, VectorContains(WSimplex{Simplex{0}, 0.0f}) &&
                              VectorContains(WSimplex{Simplex{1}, 0.0f}) &&
                              VectorContains(WSimplex{Simplex{0, 1}, 1.0f}));
}

TEST_CASE("Two points outside radius") {
    Pointcloud cloud(2);
    cloud.Add({0, 0});
    cloud.Add({3, 0});
    auto filtration = Filtration::VietorisRips(2.0, 1);
    auto generator = filtration.Build(cloud);
    std::vector<WSimplex> simplices(generator.begin(), generator.end());

    CHECK(simplices.size() == 2);
    CHECK_FALSE(
        std::any_of(simplices.begin(), simplices.end(), [](const WSimplex& s) {
            return s.simplex.size() == 2;
        }));
}

TEST_CASE("Three points forming a triangle within radius") {
    using Catch::Matchers::Approx;

    Pointcloud cloud(2);
    cloud.Add({0, 0});
    cloud.Add({1, 0});
    cloud.Add({0.5f, 0.866f});
    auto filtration = Filtration::VietorisRips(1.1f, 2);
    auto generator = filtration.Build(cloud);
    std::vector<WSimplex> simplices(generator.begin(), generator.end());

    CHECK(simplices.size() == 7);
    CHECK(std::count_if(simplices.begin(), simplices.end(),
                        [](const WSimplex& s) {
                            return s.simplex.size() == 1;
                        }) == 3);
    CHECK(std::count_if(simplices.begin(), simplices.end(),
                        [](const WSimplex& s) {
                            return s.simplex.size() == 2;
                        }) == 3);
    CHECK(std::count_if(simplices.begin(), simplices.end(),
                        [](const WSimplex& s) {
                            return s.simplex.size() == 3;
                        }) == 1);

    auto triangle_it =
        std::find_if(simplices.begin(), simplices.end(), [](const WSimplex& s) {
            return s.simplex.size() == 3;
        });
    CHECK(triangle_it != simplices.end());
    CHECK_THAT(triangle_it->weight, WithinRel(1.0f));
}

TEST_CASE("Max dimension limits simplices") {
    Pointcloud cloud(2);
    cloud.Add({0, 0});
    cloud.Add({1, 0});
    cloud.Add({0.5f, 0.866f});
    auto filtration = Filtration::VietorisRips(1.1f, 1);
    auto generator = filtration.Build(cloud);
    std::vector<WSimplex> simplices(generator.begin(), generator.end());

    CHECK(simplices.size() == 6);
    CHECK(
        std::none_of(simplices.begin(), simplices.end(), [](const WSimplex& s) {
            return s.simplex.size() > 2;
        }));
}

TEST_CASE("Higher simplex with one edge exceeding radius") {
    Pointcloud cloud(2);
    cloud.Add({0, 0});
    cloud.Add({1, 0});
    cloud.Add({3, 0});
    auto filtration = Filtration::VietorisRips(2.0f, 2);
    auto generator = filtration.Build(cloud);
    std::vector<WSimplex> simplices(generator.begin(), generator.end());

    CHECK(simplices.size() == 5);
    CHECK(std::count_if(simplices.begin(), simplices.end(),
                        [](const WSimplex& s) {
                            return s.simplex.size() == 2;
                        }) == 2);
    CHECK(
        std::none_of(simplices.begin(), simplices.end(), [](const WSimplex& s) {
            return s.simplex.size() == 3;
        }));
}

TEST_CASE("Stress test with 5 points") {
    const int N = 5;
    Pointcloud cloud(2);
    for (int i = 0; i < N; ++i) {
        cloud.Add({static_cast<float>(i), 0.0f});
    }
    auto filtration = Filtration::VietorisRips(100.0f, N - 1);
    auto generator = filtration.Build(cloud);
    size_t count = 0;
    for (const auto& s : generator) {
        REQUIRE(std::is_sorted(s.simplex.begin(), s.simplex.end()));
        ++count;
    }
    REQUIRE(count == 31);
}

TEST_CASE("Simplices are sorted correctly") {
    Pointcloud cloud(2);
    cloud.Add({0, 0});
    cloud.Add({1, 0});
    cloud.Add({2, 0});
    auto filtration = Filtration::VietorisRips(5.0f, 2);
    auto generator = filtration.Build(cloud);
    std::vector<WSimplex> simplices(generator.begin(), generator.end());

    for (size_t i = 1; i < simplices.size(); ++i) {
        const auto& prev = simplices[i - 1];
        const auto& curr = simplices[i];
        REQUIRE((prev < curr));
    }
}
