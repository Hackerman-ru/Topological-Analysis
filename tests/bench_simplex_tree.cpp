#include "simplex_tree.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <random>
#include <vector>

using namespace topa;

static std::vector<Simplex> GenerateSimplices(size_t count, size_t max_dim) {
    std::vector<Simplex> simplices;
    std::mt19937 rng(42);
    std::uniform_int_distribution<VertexId> dist(0, 1000);

    for (size_t i = 0; i < count; ++i) {
        Simplex s;
        size_t dim = 1 + (i % max_dim);
        for (size_t j = 0; j < dim; ++j) {
            s.push_back(dist(rng));
        }
        std::sort(s.begin(), s.end());
        simplices.push_back(s);
    }
    return simplices;
}

TEST_CASE("Benchmark Add Operations") {
    constexpr size_t kNumSimplices = 10'000;
    auto simplices = GenerateSimplices(kNumSimplices, 5);

    BENCHMARK("Add 10K simplices (dim 1-5)") {
        SimplexTree st(1001);
        for (size_t i = 0; i < kNumSimplices; ++i) {
            st.Add(simplices[i], i);
        }
        return st;
    };
}

TEST_CASE("Benchmark Has Operations") {
    SimplexTree st(1001);
    constexpr size_t kNumSimplices = 10'000;
    auto simplices = GenerateSimplices(kNumSimplices, 5);
    for (size_t i = 0; i < kNumSimplices; ++i) {
        st.Add(simplices[i], i);
    }

    BENCHMARK_ADVANCED("Has (hit)")(Catch::Benchmark::Chronometer meter) {
        std::mt19937 rng(42);
        std::uniform_int_distribution<size_t> dist(0, kNumSimplices - 1);

        meter.measure([&] {
            return st.Has(simplices[dist(rng)]);
        });
    };

    auto rnd_simplices = GenerateSimplices(kNumSimplices, 5);
    BENCHMARK_ADVANCED("Has (rand)")(Catch::Benchmark::Chronometer meter) {
        std::mt19937 rng(42);
        std::uniform_int_distribution<size_t> dist(0, kNumSimplices - 1);

        meter.measure([&] {
            return st.Has(rnd_simplices[dist(rng)]);
        });
    };
}

TEST_CASE("Benchmark Facet/Cofacet Operations") {
    SimplexTree st(1001);
    for (VertexId a = 0; a < 10; ++a) {
        st.Add({a}, a);
        for (VertexId b = a + 1; b < 10; ++b) {
            st.Add({a, b}, a * 10 + b);
            for (VertexId c = b + 1; c < 10; ++c) {
                st.Add({a, b, c}, a * 100 + b * 10 + c);
            }
        }
    }

    BENCHMARK("GetFacets (3-simplex)") {
        return st.GetFacets({3, 5, 7});
    };

    BENCHMARK("GetCofacets (2-simplex)") {
        return st.GetCofacets({2, 4});
    };
}

TEST_CASE("Benchmark: GetFacets for Large Simplex") {
    SimplexTree st(100);
    st.Add({0, 1, 2, 3, 4}, 100);

    st.Add({0, 1, 2, 3}, 1);
    st.Add({0, 1, 2, 4}, 2);
    st.Add({0, 1, 3, 4}, 3);
    st.Add({0, 2, 3, 4}, 4);
    st.Add({1, 2, 3, 4}, 5);

    BENCHMARK("GetFacets for 5-vertex simplex") {
        return st.GetFacets({0, 1, 2, 3, 4});
    };
}

TEST_CASE("Benchmark: GetCofacets in Dense Tree") {
    SimplexTree st(50);

    for (size_t i = 0; i < 5; ++i) {
        Simplex s;
        for (size_t j = 0; j <= i; ++j) {
            s.push_back(j);
        }
        st.Add(s, i);
    }

    BENCHMARK("GetCofacets for 3-vertex simplex") {
        return st.GetCofacets({0, 1, 2});
    };
}

TEST_CASE("Benchmark Move Operations") {
    constexpr size_t kNumSimplices = 100'000;
    auto simplices = GenerateSimplices(kNumSimplices, 3);

    BENCHMARK("Move constructor") {
        SimplexTree src(1001);
        for (size_t i = 0; i < kNumSimplices; ++i) {
            src.Add(simplices[i], i);
        }
        SimplexTree dst = std::move(src);
        return dst;
    };

    BENCHMARK("Move assignment") {
        SimplexTree src(1001);
        SimplexTree dst(0);
        for (size_t i = 0; i < kNumSimplices; ++i) {
            src.Add(simplices[i], i);
        }
        dst = std::move(src);
        return dst;
    };
}
