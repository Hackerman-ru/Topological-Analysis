#include "filtration.hpp"
#include "simplex.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace topa;

namespace {
auto count_simplices_of_dim = [](const WSimplices& simplices, size_t dim) {
    return std::count_if(simplices.begin(), simplices.end(),
                         [dim](const WSimplex& s) {
                             return s.simplex.size() == dim + 1;
                         });
};

std::vector<Simplex> generate_faces(const Simplex& s) {
    std::vector<Simplex> faces;
    if (s.size() <= 1)
        return faces;

    for (size_t i = 0; i < s.size(); ++i) {
        Simplex face;
        for (size_t j = 0; j < s.size(); ++j) {
            if (j != i)
                face.push_back(s[j]);
        }
        std::sort(face.begin(), face.end());
        faces.push_back(face);
    }
    return faces;
}

bool contains_simplex(const std::map<Simplex, Weight>& map, const Simplex& s) {
    Simplex sorted = s;
    std::sort(sorted.begin(), sorted.end());
    return map.find(sorted) != map.end();
}
}  // namespace

TEST_CASE("FullVietorisRips filtration", "[filtration][fullvr]") {
    Pointcloud cloud(2);
    cloud.Add({0, 0});
    cloud.Add({1, 0});
    cloud.Add({0, 1});

    SECTION("Complete 2-skeleton") {
        auto filtration = Filtration::FullVietorisRips();
        auto simplices = filtration.Build(cloud);

        REQUIRE(count_simplices_of_dim(simplices, 0) == 3);
        REQUIRE(count_simplices_of_dim(simplices, 1) == 3);
        REQUIRE(count_simplices_of_dim(simplices, 2) == 1);
    }
}

TEST_CASE("Stress tests", "[stress]") {
    SECTION("Large point cloud") {
        Pointcloud cloud(3);
        constexpr size_t N = 100;
        for (size_t i = 0; i < N; ++i) {
            cloud.Add({static_cast<float>(i), 0.0f, 0.0f});
        }

        auto simplices = Filtration::FullVietorisRips().Build(cloud);
        REQUIRE(count_simplices_of_dim(simplices, 1) == (N * (N - 1)) / 2);
    }
}

TEST_CASE("Weights correspond to VR-definition", "[filtration][math]") {
    auto cloud = *Pointcloud::Load(DATA_DIR "/pointclouds/klein.off");
    auto simplices = Filtration::FullVietorisRips().Build(cloud);

    for (const auto& ws : simplices) {
        Weight max_pairwise = 0.0f;
        const auto& vert = ws.simplex;

        for (size_t i = 0; i < vert.size(); ++i) {
            for (size_t j = i + 1; j < vert.size(); ++j) {
                max_pairwise = std::max(
                    max_pairwise, cloud.EuclideanDistance(vert[i], vert[j]));
            }
        }

        REQUIRE_THAT(ws.weight,
                     Catch::Matchers::WithinAbs(max_pairwise, kEpsilon));
    }
}
