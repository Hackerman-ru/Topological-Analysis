#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "fast/full_vr.hpp"

namespace topa::common {
namespace {

// Определяем мок для Pointcloud
struct MockTraits {
    using Distance = float;
    using Point = int;  // Тип точки не важен в моке
    using Points = std::vector<Point>;
};

class MockCloud : public models::Pointcloud<MockCloud, MockTraits> {
   public:
    void Add(Point point) {
        points_.push_back(point);
    }

    std::size_t Size() const {
        return points_.size();
    }

    const Points& GetPoints() const {
        return points_;
    }

    Distance GetDistance(std::size_t i, std::size_t j) const {
        // Задаем фиксированные расстояния для тестов
        if (i > j)
            std::swap(i, j);
        if (i == 0 && j == 1)
            return 1.0f;
        if (i == 0 && j == 2)
            return 2.0f;
        if (i == 1 && j == 2)
            return 3.0f;
        return 0.0f;
    }

   private:
    Points points_;
};

}  // namespace

using namespace topa;
using namespace topa::fast;

TEST_CASE("FullVR generates correct simplices count", "[FullVR]") {
    FullVR vr;
    MockCloud cloud;

    SECTION("Single point cloud") {
        cloud.Add(1);
        auto simplices = vr.Filter(cloud);
        REQUIRE(simplices.size() == 1);  // Только вершина
    }

    SECTION("Two points cloud") {
        cloud.Add(1);
        cloud.Add(2);
        auto simplices = vr.Filter(cloud);
        REQUIRE(simplices.size() == 3);  // 2 вершины + 1 ребро
    }

    SECTION("Three points cloud") {
        cloud.Add(1);
        cloud.Add(2);
        cloud.Add(3);
        auto simplices = vr.Filter(cloud);
        REQUIRE(simplices.size() == 7);  // 3 вершины + 3 ребра + 1 треугольник
    }
}

TEST_CASE("FullVR computes correct filtration_values", "[FullVR]") {
    FullVR vr;
    MockCloud cloud;

    cloud.Add(1);
    cloud.Add(2);
    cloud.Add(3);
    auto simplices = vr.Filter(cloud);

    // Проверяем вес треугольника (0,1,2)
    const auto& triangle = simplices.back();
    REQUIRE(triangle.GetFiltrationValue() == 3.0f);
}

TEST_CASE("FullVR sorts simplices correctly", "[FullVR]") {
    FullVR vr;
    MockCloud cloud;

    cloud.Add(1);
    cloud.Add(2);
    cloud.Add(3);
    auto simplices = vr.Filter(cloud);

    // Ожидаемый порядок: вершины (вес 0), ребра (1, 2, 3), треугольник (3)
    REQUIRE(simplices[0].GetFiltrationValue() == 0.0f);
    REQUIRE(simplices[1].GetFiltrationValue() == 0.0f);
    REQUIRE(simplices[2].GetFiltrationValue() == 0.0f);

    REQUIRE(simplices[3].GetFiltrationValue() == 1.0f);
    REQUIRE(simplices[4].GetFiltrationValue() == 2.0f);
    REQUIRE(simplices[5].GetFiltrationValue() == 3.0f);

    REQUIRE(simplices[6].GetFiltrationValue() == 3.0f);
    REQUIRE(simplices[6].Dim() == 2);
}

TEST_CASE("FullVR creates correct simplex structure", "[FullVR]") {
    FullVR vr;
    MockCloud cloud;

    cloud.Add(1);
    cloud.Add(2);
    cloud.Add(3);
    auto simplices = vr.Filter(cloud);

    // Проверяем вершины
    REQUIRE(simplices[0].GetSimplex() == Simplex{0});
    REQUIRE(simplices[1].GetSimplex() == Simplex{1});
    REQUIRE(simplices[2].GetSimplex() == Simplex{2});

    // Проверяем ребра
    REQUIRE(simplices[3].GetSimplex() == Simplex{0, 1});
    REQUIRE(simplices[4].GetSimplex() == Simplex{0, 2});
    REQUIRE(simplices[5].GetSimplex() == Simplex{1, 2});

    // Проверяем треугольник
    REQUIRE(simplices[6].GetSimplex() == Simplex{0, 1, 2});
}

}  // namespace topa::common
