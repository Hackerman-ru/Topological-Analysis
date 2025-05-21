#include "topa/topa.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

// Определение тестового облака точек
struct TestTraits {
    using Distance = float;
    using Point = std::pair<float, float>;
    using Points = std::vector<Point>;
};

class TestPointcloud : public models::Pointcloud<TestPointcloud, TestTraits> {
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

   private:
    Points points_;
};

class Distance : public models::Distance<Distance> {
   public:
    static FiltrationValue GetDistance(std::pair<float, float> lhs,
                                       std::pair<float, float> rhs) {
        float dx = lhs.first - rhs.first;
        float dy = lhs.second - rhs.second;
        return std::sqrt(dx * dx + dy * dy);
    }
};

#define TEST(name) \
    TEMPLATE_TEST_CASE_SIG(name, "", ((size_t N), N), 25, 100, 250)

// Проверка корректности результатов
TEST_CASE("Correctness of NewVR and FullVR") {
    TestPointcloud cloud;
    cloud.Add({0.0f, 0.0f});
    cloud.Add({1.0f, 0.0f});
    cloud.Add({0.0f, 1.0f});

    const auto& points = cloud.GetPoints();

    // Вычисляем максимальное расстояние для включения всех рёбер
    float max_distance = 0.0f;
    for (size_t i = 0; i < cloud.Size(); ++i) {
        for (size_t j = i + 1; j < cloud.Size(); ++j) {
            max_distance = std::max(
                max_distance, Distance::GetDistance(points[i], points[j]));
        }
    }
    const FiltrationValue max_radius = max_distance + 0.1f;
    const size_t max_dim = 2;

    NewVR<Distance> new_vr(max_radius, max_dim);
    auto new_result = new_vr.Filter(cloud);

    FullVR<Distance> full_vr;
    auto full_result = full_vr.Filter(cloud);

    REQUIRE(new_result.size() == full_result.size());
    REQUIRE(new_result == full_result);
}

// Бенчмарки для разных размеров облаков
TEST("Benchmark VR Implementations") {
    TestPointcloud cloud;
    // Генерация случайных точек
    for (size_t i = 0; i < N; ++i) {
        float x = static_cast<float>(rand()) / RAND_MAX * 10.0f;
        float y = static_cast<float>(rand()) / RAND_MAX * 10.0f;
        cloud.Add({x, y});
    }

    const auto& points = cloud.GetPoints();

    // Определение max_radius для полного графа
    FiltrationValue max_distance = 0.0f;
    for (size_t i = 0; i < cloud.Size(); ++i) {
        for (size_t j = i + 1; j < cloud.Size(); ++j) {
            max_distance = std::max(
                max_distance, Distance::GetDistance(points[i], points[j]));
        }
    }
    const FiltrationValue max_radius =
        max_distance + static_cast<FiltrationValue>(0.1f);
    const size_t max_dim = 2;

    BENCHMARK("NewVR") {
        NewVR<Distance> vr(max_radius, max_dim);
        return vr.Filter(cloud);
    };

    BENCHMARK("FullVR") {
        FullVR<Distance> vr;
        return vr.Filter(cloud);
    };
}
