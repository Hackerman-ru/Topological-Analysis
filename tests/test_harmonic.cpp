#include "topa/topa.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

TEST_CASE("Harmonic Test (361)") {
    using Solver = ShiftSolver<25, 50, 15000, 6>;
    Pointcloud cloud =
        Pointcloud::Load(DATA_DIR "/pointclouds/klein.off").value();
    auto complex =
        FilteredComplex<FlatTree>::From(cloud, FullVR<EucledianDistance>());
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
    auto harmonic_cycles =
        Harmonic<Solver, SVDSeparator, 1>::Compute(complex, pairs);
    detail::PrintCycles(harmonic_cycles);
}

TEST_CASE("Harmonic Test (525)") {
    using Solver = ShiftSolver<25, 50, 15000, 6>;
    Pointcloud cloud =
        Pointcloud::Load(DATA_DIR "/pointclouds/525.off").value();
    auto complex =
        FilteredComplex<FlatTree>::From(cloud, FullVR<EucledianDistance>());
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
    auto harmonic_cycles =
        Harmonic<Solver, SVDSeparator, 10>::Compute(complex, pairs);
    detail::PrintCycles(harmonic_cycles);
}
