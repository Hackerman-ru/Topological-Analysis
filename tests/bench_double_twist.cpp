#include "fast/double_twist.hpp"

#include "common/detail/flat_matrix.hpp"
#include "common/simplex_tree.hpp"
#include "fast/full_tree_opt.hpp"
#include "common/filtered_complex.hpp"
#include "fast/bit_tree_heap.hpp"
#include "fast/sparse_matrix.hpp"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include <random>

using namespace topa;
using namespace topa::fast;
using namespace topa::common;

namespace {

template <typename Complex>
Complex GenerateRandomComplex(VertexId n_vertices, Weight max_radius) {
    std::vector<WSimplex> wsimplices;

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist_randomizer(0.0f, 10.0f);

    detail::FlatMatrix<Weight> dist(n_vertices, n_vertices);
    for (VertexId i = 0; i < n_vertices; ++i) {
        for (VertexId j = i + 1; j < n_vertices; ++j) {
            dist[i][j] = static_cast<Weight>(dist_randomizer(gen));
        }
    }

    const std::size_t total =
        n_vertices + n_vertices * (n_vertices - 1) / 2 +
        n_vertices * (n_vertices - 1) * (n_vertices - 2) / 6;
    wsimplices.reserve(total / 2);

    for (VertexId i = 0; i < n_vertices; ++i) {
        wsimplices.emplace_back(Simplex{i}, 0.0f);
        for (VertexId j = i + 1; j < n_vertices; ++j) {
            const Weight d_ij = dist[i][j];
            if (d_ij > max_radius) {
                continue;
            }
            wsimplices.emplace_back(Simplex{i, j}, d_ij);
            for (VertexId k = j + 1; k < n_vertices; ++k) {
                const Weight max_d = std::max({d_ij, dist[i][k], dist[j][k]});
                if (max_d <= max_radius) {
                    wsimplices.emplace_back(Simplex{i, j, k}, max_d);
                }
            }
        }
    }

    std::sort(wsimplices.begin(), wsimplices.end());
    return Complex(std::move(wsimplices), n_vertices);
}

#define TEST(name) \
    TEMPLATE_TEST_CASE_SIG(name, "", ((size_t N), N), 50, 100, 250)

}  // namespace

TEST("Random 2-skeleton") {
    const auto complex =
        GenerateRandomComplex<FilteredComplex<SimplexTree>>(N, 5.0f);
    BENCHMARK("DoubleTwist") {
        DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
    };
}

TEST("Full 2-skeleton") {
    const auto complex =
        GenerateRandomComplex<FilteredComplex<FullTreeOpt>>(N, 11.0f);
    BENCHMARK("DoubleTwist") {
        DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
    };
}
