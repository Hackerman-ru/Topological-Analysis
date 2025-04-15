#include "simplex_tree.hpp"

#include <benchmark/benchmark.h>

using namespace topa;

static void BM_AddSimplex(benchmark::State& state) {
    SimplexTree st(100);
    const int size = state.range(0);

    for (auto _ : state) {
        SimplexTree::SortedSimplex s;
        for (SimplexTree::VertexId i = 0; i < size; ++i) {
            s.push_back(i);
        }
        st.add_simplex(s, size * 1.0);
    }
}

BENCHMARK(BM_AddSimplex)->Arg(2)->Arg(10)->Arg(50);

static void BM_HasSimplex(benchmark::State& state) {
    const int N = 100000;
    SimplexTree st(N);

    std::vector<SimplexTree::SortedSimplex> simplices;
    for (int i = 0; i < N; ++i) {
        simplices.push_back({i, i + 1, i + 2});
        st.add_simplex(simplices.back(), 1.0);
    }

    size_t idx = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(st.has_simplex(simplices[idx++ % N]));
    }
}

BENCHMARK(BM_HasSimplex);

static void BM_Facets(benchmark::State& state) {
    const int depth = state.range(0);
    SimplexTree st(depth);

    SimplexTree::SortedSimplex s;
    for (SimplexTree::VertexId i = 0; i < depth; ++i) {
        s.push_back(i);
        st.add_simplex(s, i + 1.0);
    }

    for (auto _ : state) {
        auto facets = st.facets(s);
        benchmark::DoNotOptimize(facets);
    }
}

BENCHMARK(BM_Facets)->Arg(10)->Arg(100)->Arg(1000);

static void BM_Cofacets(benchmark::State& state) {
    const int width = state.range(0);
    SimplexTree st(width + 1);

    st.add_simplex({0}, 1.0);
    for (SimplexTree::VertexId i = 1; i <= width; ++i) {
        st.add_simplex({0, i}, 2.0);
    }

    SimplexTree::SortedSimplex root = {0};
    for (auto _ : state) {
        auto cofacets = st.cofacets(root);
        benchmark::DoNotOptimize(cofacets);
    }
}

BENCHMARK(BM_Cofacets)->Arg(10)->Arg(1000)->Arg(10000);

BENCHMARK_MAIN();
