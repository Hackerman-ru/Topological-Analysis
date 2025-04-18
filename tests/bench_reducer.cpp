#include "reducer.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

using namespace topa;

TEST_CASE("Double twist scaling", "[stress][timing]") {
    auto pointcloud = Pointcloud::Load(DATA_DIR "/pointclouds/325.off");
    Filtration filtration = Filtration::FullVietorisRips();
    FilteredComplex complex = FilteredComplex::From(*pointcloud, filtration);
    BENCHMARK("Size = 5,721,625") {
        Reducer::DoubleTwist().Reduce(complex);
    };

    pointcloud = Pointcloud::Load(DATA_DIR "/pointclouds/525.off");
    complex = FilteredComplex::From(*pointcloud, filtration);
    BENCHMARK("Size = 24,117,625") {
        Reducer::DoubleTwist().Reduce(complex);
    };

    pointcloud = Pointcloud::Load(DATA_DIR "/pointclouds/591.off");
    complex = FilteredComplex::From(*pointcloud, filtration);
    BENCHMARK("Size = 34,404,671") {
        Reducer::DoubleTwist().Reduce(complex);
    };
}

TEST_CASE("Reduce strategies comparison", "[stress][timing]") {
    auto pointcloud = Pointcloud::Load(DATA_DIR "/pointclouds/325.off");
    Filtration filtration = Filtration::FullVietorisRips();
    FilteredComplex complex = FilteredComplex::From(*pointcloud, filtration);

    BENCHMARK("Twist") {
        Reducer::Twist().Reduce(complex);
    };

    BENCHMARK("DoubleTwist") {
        Reducer::DoubleTwist().Reduce(complex);
    };
}
