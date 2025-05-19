#include "common/pointcloud.hpp"
#include "common/eucledian_distance.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace topa;
using namespace topa::common;
using namespace Catch::Matchers;

TEST_CASE("Pointcloud basic functionality", "[Pointcloud]") {
    SECTION("Empty cloud") {
        Pointcloud cloud(0);
        REQUIRE(cloud.Size() == 0);
        REQUIRE(cloud.GetPoints().size() == 0);
    }

    const size_t DIM = 3;
    Pointcloud cloud(DIM);

    SECTION("Initialization") {
        REQUIRE(cloud.Size() == 0);
        REQUIRE(cloud.GetPoints().size() == 0);
    }

    SECTION("Adding points") {
        Pointcloud::Point p1{1.0, 2.0, 3.0};
        Pointcloud::Point p2{4.0, 5.0, 6.0};

        cloud.Add(p1);
        REQUIRE(cloud.Size() == 1);

        cloud.Add(p2);
        REQUIRE(cloud.Size() == 2);

        const auto& points = cloud.GetPoints();
        REQUIRE(points[0][0] == 1.0);
        REQUIRE(points[1][2] == 6.0);
    }

    SECTION("Distance calculations") {
        cloud.Add({0.0, 0.0, 0.0});
        cloud.Add({3.0, 4.0, 0.0});
        auto points = cloud.GetPoints();

        REQUIRE_THAT(EucledianDistance::GetDistance(points[0], points[1]),
                     WithinAbs(25.0, 1e-6));
    }
}

TEST_CASE("Pointcloud file loading", "[Pointcloud]") {
    SECTION("Successful load") {
        auto cloud = Pointcloud::Load(DATA_DIR "/pointclouds/klein.off");
        REQUIRE(cloud.has_value());

        const auto& points = cloud->GetPoints();
        REQUIRE(points.size() > 0);
        REQUIRE(points[0].size() == 3);
    }

    SECTION("Failed load") {
        auto cloud = Pointcloud::Load("nonexistent_file.off");
        REQUIRE_FALSE(cloud.has_value());
    }
}

TEST_CASE("Stress tests for Pointcloud", "[Pointcloud][stress]") {
    const size_t DIM = 3;
    const size_t NUM_POINTS = 100000;
    Pointcloud cloud(DIM);

    SECTION("Massive point addition") {
        for (size_t i = 0; i < NUM_POINTS; ++i) {
            cloud.Add({static_cast<Pointcloud::CoordinateType>(i),
                       static_cast<Pointcloud::CoordinateType>(i + 1),
                       static_cast<Pointcloud::CoordinateType>(i + 2)});
        }
        REQUIRE(cloud.Size() == NUM_POINTS);
    }

    SECTION("Distance calculations performance") {
        for (size_t i = 0; i < 1000; ++i) {
            cloud.Add({static_cast<Pointcloud::CoordinateType>(i),
                       static_cast<Pointcloud::CoordinateType>(i),
                       static_cast<Pointcloud::CoordinateType>(i)});
        }

        auto points = cloud.GetPoints();
        for (size_t i = 0; i < 1000; ++i) {
            for (size_t j = i + 1; j < 1000; ++j) {
                auto dist =
                    EucledianDistance::GetDistance(points[i], points[j]);
                REQUIRE(dist >= 0);
            }
        }
    }
}
