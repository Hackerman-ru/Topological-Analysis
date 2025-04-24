#include "common/new_vr.hpp"
#include "fast/full_vr.hpp"
#include "models/pointcloud.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

namespace topa::models {

// Определение тестового облака точек
struct TestTraits {
    using Distance = float;
    using Point = std::pair<float, float>;
    using Points = std::vector<Point>;
};

class TestPointcloud : public Pointcloud<TestPointcloud, TestTraits> {
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
        const auto& p1 = points_[i];
        const auto& p2 = points_[j];
        float dx = p1.first - p2.first;
        float dy = p1.second - p2.second;
        return std::sqrt(dx * dx + dy * dy);
    }

   private:
    Points points_;
};

#define TEST(name) \
    TEMPLATE_TEST_CASE_SIG(name, "", ((size_t N), N), 25, 100, 250)

}  // namespace topa::models

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

// Проверка корректности результатов
TEST_CASE("Correctness of NewVR and FullVR") {
    topa::models::TestPointcloud cloud;
    cloud.Add({0.0f, 0.0f});
    cloud.Add({1.0f, 0.0f});
    cloud.Add({0.0f, 1.0f});

    // Вычисляем максимальное расстояние для включения всех рёбер
    float max_distance = 0.0f;
    for (size_t i = 0; i < cloud.Size(); ++i) {
        for (size_t j = i + 1; j < cloud.Size(); ++j) {
            max_distance = std::max(max_distance, cloud.GetDistance(i, j));
        }
    }
    const FiltrationValue max_radius = max_distance + 0.1f;
    const size_t max_dim = 2;

    topa::common::NewVR new_vr(max_radius, max_dim);
    auto new_result = new_vr.Filter(cloud);

    topa::fast::FullVR full_vr;
    auto full_result = full_vr.Filter(cloud);

    REQUIRE(new_result.size() == full_result.size());
    REQUIRE(new_result == full_result);
}

// Бенчмарки для разных размеров облаков
TEST("Benchmark VR Implementations") {
    topa::models::TestPointcloud cloud;
    // Генерация случайных точек
    for (size_t i = 0; i < N; ++i) {
        float x = static_cast<float>(rand()) / RAND_MAX * 10.0f;
        float y = static_cast<float>(rand()) / RAND_MAX * 10.0f;
        cloud.Add({x, y});
    }

    // Определение max_radius для полного графа
    FiltrationValue max_distance = 0.0f;
    for (size_t i = 0; i < cloud.Size(); ++i) {
        for (size_t j = i + 1; j < cloud.Size(); ++j) {
            max_distance =
                std::max(max_distance,
                         static_cast<FiltrationValue>(cloud.GetDistance(i, j)));
        }
    }
    const FiltrationValue max_radius =
        max_distance + static_cast<FiltrationValue>(0.1f);
    const size_t max_dim = 2;

    BENCHMARK("NewVR") {
        topa::common::NewVR vr(max_radius, max_dim);
        return vr.Filter(cloud);
    };

    BENCHMARK("FullVR") {
        topa::fast::FullVR vr;
        return vr.Filter(cloud);
    };
}
