#include "filtration.hpp"
#include "pointcloud.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

using namespace topa;

TEST_CASE("VR complex construction time profiling", "[stress][timing]") {
    auto cloud = *Pointcloud::Load(DATA_DIR "/pointclouds/klein.off");

    BENCHMARK("FullVietorisRips (size=361)") {
        return Filtration::FullVietorisRips().Build(cloud);
    };

    cloud = *Pointcloud::Load(DATA_DIR "/pointclouds/dragon.off");

    BENCHMARK("FullVietorisRips (size=994)") {
        return Filtration::FullVietorisRips().Build(cloud);
    };
}
