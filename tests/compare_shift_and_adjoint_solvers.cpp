#include "fast/sai_solver.hpp"
#include "fast/sa_solver.hpp"

#include "common/pointcloud.hpp"
#include "common/eucledian_distance.hpp"
#include "common/filtered_complex.hpp"
#include "common/svd_separator.hpp"
#include "common/detail/harmonic_printer.hpp"
#include "fast/harmonic.hpp"
#include "fast/full_vr.hpp"
#include "fast/full_tree_opt.hpp"
#include "fast/double_twist.hpp"
#include "fast/bit_tree_heap.hpp"
#include "fast/sparse_matrix.hpp"

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
