#include "topa/topa.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

using namespace topa;
using namespace topa::fast;

namespace {

std::vector<std::vector<VertexId>> generate_simplices(size_t n) {
    std::vector<std::vector<VertexId>> simplices;

    // Вершины
    for (VertexId v = 0; v < n; ++v) {
        simplices.push_back({v});
    }

    // Рёбра
    for (VertexId u = 0; u < n; ++u) {
        for (VertexId v = u + 1; v < n; ++v) {
            simplices.push_back({u, v});
        }
    }

    // Треугольники
    for (VertexId u = 0; u < n; ++u) {
        for (VertexId v = u + 1; v < n; ++v) {
            for (VertexId w = v + 1; w < n; ++w) {
                simplices.push_back({u, v, w});
            }
        }
    }

    return simplices;
}

#define TEST(name) \
    TEMPLATE_TEST_CASE_SIG(name, "", ((size_t N), N), 50, 250, 1000)

}  // namespace

TEST("Benchmark Add") {
    auto simplices = generate_simplices(N);

    BENCHMARK("FullTree::Add") {
        FullTree tree(N);
        for (const auto& simplex : simplices) {
            tree.Add(simplex, 0);
        }
        return tree;
    };

    BENCHMARK("FullTreeOpt::Add") {
        FullTreeOpt tree(N);
        for (const auto& simplex : simplices) {
            tree.Add(simplex, 0);
        }
        return tree;
    };
}

TEST("Benchmark Has") {
    auto simplices = generate_simplices(N);

    FullTree full_tree(N);
    FullTreeOpt full_tree_opt(N);
    for (const auto& simplex : simplices) {
        full_tree.Add(simplex, 0);
        full_tree_opt.Add(simplex, 0);
    }

    BENCHMARK("FullTree::Has") {
        size_t count = 0;
        for (const auto& simplex : simplices) {
            count += full_tree.Has(simplex);
        }
        return count;
    };

    BENCHMARK("FullTreeOpt::Has") {
        size_t count = 0;
        for (const auto& simplex : simplices) {
            count += full_tree_opt.Has(simplex);
        }
        return count;
    };
}

TEST("Benchmark GetPos") {
    auto simplices = generate_simplices(N);

    FullTree full_tree(N);
    FullTreeOpt full_tree_opt(N);
    for (const auto& simplex : simplices) {
        full_tree.Add(simplex, 0);
        full_tree_opt.Add(simplex, 0);
    }

    BENCHMARK("FullTree::GetPos") {
        size_t count = 0;
        for (const auto& simplex : simplices) {
            count += full_tree.GetPos(simplex) != kUnknownPos;
        }
        return count;
    };

    BENCHMARK("FullTreeOpt::GetPos") {
        size_t count = 0;
        for (const auto& simplex : simplices) {
            count += full_tree_opt.GetPos(simplex) != kUnknownPos;
        }
        return count;
    };
}

TEST("Benchmark GetFacetsPos") {
    auto simplices = generate_simplices(N);

    FullTree full_tree(N);
    FullTreeOpt full_tree_opt(N);
    for (const auto& simplex : simplices) {
        full_tree.Add(simplex, 0);
        full_tree_opt.Add(simplex, 0);
    }

    std::vector<std::vector<VertexId>> triangles;
    for (const auto& s : simplices) {
        if (s.size() == 3) {
            triangles.push_back(s);
        }
    }

    BENCHMARK("FullTree::GetFacetsPos (triangles)") {
        size_t total = 0;
        for (const auto& tri : triangles) {
            auto facets = full_tree.GetFacetsPos(tri);
            total += facets.size();
        }
        return total;
    };

    BENCHMARK("FullTreeOpt::GetFacetsPos (triangles)") {
        size_t total = 0;
        for (const auto& tri : triangles) {
            auto facets = full_tree_opt.GetFacetsPos(tri);
            total += facets.size();
        }
        return total;
    };
}

TEST("Benchmark GetCofacetsPos") {
    auto simplices = generate_simplices(N);

    FullTree full_tree(N);
    FullTreeOpt full_tree_opt(N);
    for (const auto& simplex : simplices) {
        full_tree.Add(simplex, 0);
        full_tree_opt.Add(simplex, 0);
    }

    std::vector<std::vector<VertexId>> edges;
    for (const auto& s : simplices) {
        if (s.size() == 2) {
            edges.push_back(s);
        }
    }

    BENCHMARK("FullTree::GetCofacetsPos (edges)") {
        size_t total = 0;
        for (const auto& edge : edges) {
            auto cofacets = full_tree.GetCofacetsPos(edge);
            total += cofacets.size();
        }
        return total;
    };

    BENCHMARK("FullTreeOpt::GetCofacetsPos (edges)") {
        size_t total = 0;
        for (const auto& edge : edges) {
            auto cofacets = full_tree_opt.GetCofacetsPos(edge);
            total += cofacets.size();
        }
        return total;
    };
}
