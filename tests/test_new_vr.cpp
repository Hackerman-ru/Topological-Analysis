#include "common/new_vr.hpp"

#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

namespace {

struct TestCloudTraits {
    using CoordinateType = float;
    using Distance = CoordinateType;
    using Point = std::vector<CoordinateType>;
    using Points = std::vector<std::vector<float>>;
};

// Тестовая реализация CloudImpl
class TestCloud : public topa::models::Pointcloud<TestCloud, TestCloudTraits> {
   public:
    TestCloud(const std::vector<std::vector<float>>& points)
        : points_(points) {
    }

    std::size_t Size() const {
        return points_.size();
    }

    Distance GetDistance(topa::VertexId u, topa::VertexId v) const {
        float dist = 0.0f;
        for (size_t i = 0; i < points_[u].size(); ++i) {
            float diff = points_[u][i] - points_[v][i];
            dist += diff * diff;
        }
        return std::sqrt(dist);
    }

   private:
    Points points_;
};

}  // namespace

TEST_CASE("NewVR filtration basic functionality", "[newvr]") {
    using namespace topa;
    using namespace topa::common;

    SECTION("Empty point cloud returns empty filtration") {
        TestCloud cloud({});
        NewVR vr(1.0f, 1);
        auto simplices = vr.Filter(cloud);

        REQUIRE(simplices.empty());
    }

    SECTION("Single point generates only 0-simplex") {
        TestCloud cloud({{0.0f, 0.0f}});
        NewVR vr(1.0f, 2);
        auto simplices = vr.Filter(cloud);

        REQUIRE(simplices.size() == 1);
        CHECK(simplices[0].GetSimplex() == Simplex{0});
        CHECK(simplices[0].GetWeight() == 0.0f);
    }

    SECTION("Two points within radius form edge") {
        TestCloud cloud({{0.0f}, {1.0f}});  // Расстояние 1.0
        NewVR vr(1.0f, 1);
        auto simplices = vr.Filter(cloud);

        REQUIRE(simplices.size() == 3);

        WSimplex s0({0}, 0.0f);
        WSimplex s1({1}, 0.0f);
        WSimplex s01({0, 1}, 1.0f);

        CHECK(std::find(simplices.begin(), simplices.end(), s0) !=
              simplices.end());
        CHECK(std::find(simplices.begin(), simplices.end(), s1) !=
              simplices.end());
        CHECK(std::find(simplices.begin(), simplices.end(), s01) !=
              simplices.end());
    }

    SECTION("Max radius limits edges") {
        TestCloud cloud({{0.0f}, {2.0f}});  // Расстояние 2.0
        NewVR vr(1.9f, 1);
        auto simplices = vr.Filter(cloud);

        REQUIRE(simplices.size() == 2);  // Только 0-симплексы
    }

    SECTION("Three points form correct simplices") {
        TestCloud cloud({
            {0.0f, 0.0f}, {1.0f, 0.0f}, {0.5f, 0.866f}
            // Равносторонний треугольник с длиной стороны ~1.0
        });

        NewVR vr(1.05f, 2);  // Достаточно для всех рёбер
        auto simplices = vr.Filter(cloud);

        // Ожидаем 7 симплексов: 3 точки, 3 ребра, 1 треугольник
        REQUIRE(simplices.size() == 7);

        // Проверяем наличие треугольника
        WSimplex tri({0, 1, 2}, 1.0f);
        CHECK(std::find(simplices.begin(), simplices.end(), tri) !=
              simplices.end());
    }

    SECTION("Max dimension limits simplex size") {
        TestCloud cloud({{0.0f, 0.0f}, {1.0f, 0.0f}, {0.5f, 0.866f}});

        NewVR vr(1.05f, 1);  // Только до 1-мерных
        auto simplices = vr.Filter(cloud);

        // 3 точки + 3 ребра = 6 симплексов
        REQUIRE(simplices.size() == 6);

        // Проверяем отсутствие треугольников
        auto it =
            std::find_if(simplices.begin(), simplices.end(), [](const auto& s) {
                return s.Dim() == 2;
            });
        CHECK(it == simplices.end());
    }

    SECTION("Simplices are sorted correctly") {
        TestCloud cloud({
            {0.0f},
            {2.0f},  // Расстояние 2.0 от 0
            {1.0f}   // Расстояние 1.0 от 0 и 1.0 от 1
        });

        NewVR vr(2.1f, 2);
        auto simplices = vr.Filter(cloud);

        // Проверяем порядок сортировки:
        // 1. По весу (возрастание)
        // 2. По размеру (возрастание)
        // 3. Лексикографический порядок
        std::vector<WSimplex> expected = {
            {{0}, 0.0f},    {{1}, 0.0f},    {{2}, 0.0f},      {{0, 2}, 1.0f},
            {{1, 2}, 1.0f}, {{0, 1}, 2.0f}, {{0, 1, 2}, 2.0f}};

        REQUIRE(simplices.size() == expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            CHECK(simplices[i] == expected[i]);
        }
    }
}
