#include "fast/twist.hpp"
#include "fast/double_twist.hpp"

#include "common/pointcloud.hpp"
#include "common/eucledian_distance.hpp"
#include "fast/full_vr.hpp"
#include "common/new_vr.hpp"
#include "common/filtered_complex.hpp"
#include "fast/flat_tree.hpp"
#include "fast/double_twist.hpp"
#include "fast/sparse_matrix.hpp"
#include "fast/bit_tree_heap.hpp"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace topa;
using namespace topa::fast;
using namespace topa::common;

void ComputePairs(std::string path) {
    auto pointcloud = *Pointcloud::Load(path);
    auto filration = NewVR<EucledianDistance>(0.5f, 2);
    auto complex = FilteredComplex<FlatTree>::From(pointcloud, filration);
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
}

void ComputeFullPairs(std::string path) {
    auto pointcloud = *Pointcloud::Load(path);
    auto filration = FullVR<EucledianDistance>();
    auto complex = FilteredComplex<FlatTree>::From(pointcloud, filration);
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
}

TEST_CASE("Bench peristence diagram computation") {
    SECTION("VR") {
        BENCHMARK("100 points") {
            ComputePairs(DATA_DIR "/pointclouds/random_100.off");
        };
        BENCHMARK("200 points") {
            ComputePairs(DATA_DIR "/pointclouds/random_200.off");
        };
        BENCHMARK("300 points") {
            ComputePairs(DATA_DIR "/pointclouds/random_300.off");
        };
        BENCHMARK("400 points") {
            ComputePairs(DATA_DIR "/pointclouds/random_400.off");
        };
        BENCHMARK("500 points") {
            ComputePairs(DATA_DIR "/pointclouds/random_500.off");
        };
    }
    SECTION("Full-VR") {
        BENCHMARK("100 points") {
            ComputeFullPairs(DATA_DIR "/pointclouds/random_100.off");
        };
        BENCHMARK("200 points") {
            ComputeFullPairs(DATA_DIR "/pointclouds/random_200.off");
        };
        BENCHMARK("300 points") {
            ComputeFullPairs(DATA_DIR "/pointclouds/random_300.off");
        };
        BENCHMARK("400 points") {
            ComputeFullPairs(DATA_DIR "/pointclouds/random_400.off");
        };
        BENCHMARK("500 points") {
            ComputeFullPairs(DATA_DIR "/pointclouds/random_500.off");
        };
    }
}
