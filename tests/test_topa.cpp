#include "topa/topa.hpp"

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

// 2-skeleton with max radius
void test_vr() {
    const float max_radius = 0.5f;
    const size_t max_dim = 2;
    std::string path = "Path-to-file.off";

    auto pointcloud = Pointcloud::Load(path).value();
    auto filtration = NewVR<EucledianDistance>(max_radius, max_dim);
    auto complex = FilteredComplex<FlatTree>::From(pointcloud, filtration);
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
}

// Full 2-skeleton
void test_full() {
    std::string path = "Path-to-file.off";

    auto pointcloud = Pointcloud::Load(path).value();
    auto filtration = FullVR<EucledianDistance>();
    auto complex = FilteredComplex<FlatTree>::From(pointcloud, filtration);
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
}

void test_harmonic() {
    using Solver = ShiftSolver<25, 50, 15000, 6>;
    const float max_radius = 0.5f;
    const size_t max_dim = 2;
    std::string path = "Path-to-file.off";

    auto pointcloud = Pointcloud::Load(path).value();
    auto filtration = NewVR<EucledianDistance>(max_radius, max_dim);
    auto complex = FilteredComplex<FlatTree>::From(pointcloud, filtration);
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
    auto harmonic_cycles =
        Harmonic<Solver, SVDSeparator, 10>::Compute(complex, pairs);
    detail::PrintCycles(harmonic_cycles);
}