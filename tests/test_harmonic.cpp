#include "fast/harmonic.hpp"

#include "common/pointcloud.hpp"
#include "common/filtered_complex.hpp"
#include "common/svd_separator.hpp"
#include "common/detail/harmonic_printer.hpp"
#include "fast/full_vr.hpp"
#include "fast/full_tree_opt.hpp"
#include "fast/double_twist.hpp"
#include "fast/bit_tree_heap.hpp"
#include "fast/sparse_matrix.hpp"
#include "fast/sai_solver.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

TEST_CASE("Harmonic Test") {
    Pointcloud cloud =
        Pointcloud::Load(DATA_DIR "/pointclouds/100.off").value();
    auto complex = FilteredComplex<FullTreeOpt>::From(cloud, FullVR());
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
    auto harmonic_cycles =
        Harmonic<ShiftSolver<>, SVDSeparator, 100>::Compute(complex, pairs);
    detail::PrintCycles(harmonic_cycles);
}
