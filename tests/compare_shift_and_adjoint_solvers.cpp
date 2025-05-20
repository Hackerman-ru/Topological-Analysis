#include "topa/topa.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

TEST_CASE("Solver comparisons") {
    Pointcloud cloud =
        Pointcloud::Load(DATA_DIR "/pointclouds/100.off").value();
    std::cout << "Pointcloud size = " << cloud.Size() << '\n';
    auto complex =
        FilteredComplex<FullTreeOpt>::From(cloud, FullVR<EucledianDistance>());
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);

    BENCHMARK("SelfAdjointSolver") {
        Harmonic<SelfAdjointSolver<>, SVDSeparator, 100>::Compute(complex,
                                                                  pairs);
    };
    BENCHMARK("ShiftSolver") {
        Harmonic<ShiftSolver<>, SVDSeparator, 100>::Compute(complex, pairs);
    };
}
