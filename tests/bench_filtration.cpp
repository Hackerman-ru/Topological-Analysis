#include "filtration.hpp"
#include "pointcloud.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

using namespace topa;

TEST_CASE("VR complex construction time profiling", "[stress][timing]") {
    auto cloud = *Pointcloud::Load(DATA_DIR "/pointclouds/klein.off");

    BENCHMARK("VietorisRips (r=0.3, dim=3)") {
        return Filtration::VietorisRips(0.3f, 3).Build(cloud);
    };

    BENCHMARK("VietorisRips (r=1.0, dim=5)") {
        return Filtration::VietorisRips(1.0f, 5).Build(cloud);
    };

    BENCHMARK("FullVietorisRips (size=361)") {
        return Filtration::FullVietorisRips().Build(cloud);
    };

    cloud = *Pointcloud::Load(DATA_DIR "/pointclouds/dragon.off");

    BENCHMARK("FullVietorisRips (size=994)") {
        return Filtration::FullVietorisRips().Build(cloud);
    };
}
