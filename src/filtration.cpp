#include "filtration.hpp"

#include <algorithm>
#include <unordered_set>
#ifdef TOPA_USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/enumerable_thread_specific.h>
#endif

namespace topa {

namespace {

using Graph = basic_types::DefaultMap<VertexId, std::vector<VertexId>>;

Graph BuildAdjacencyGraph(const Pointcloud& cloud, Weight max_radius) {
    Graph graph;
    const auto cloud_size = static_cast<VertexId>(cloud.Size());
    Weight sq_max_radius = max_radius * max_radius;

    for (VertexId u = 0; u < cloud_size; ++u) {
        for (VertexId v = u + 1; v < cloud_size; ++v) {
            if (cloud.SquaredEucledianDistance(u, v) <= sq_max_radius) {
                graph[u].push_back(v);
            }
        }
    }

    return graph;
}

std::vector<VertexId> UpperNeighbors(const Graph& graph, VertexId u) {
    auto it = graph.find(u);
    return (it != graph.end()) ? it->second : std::vector<VertexId>{};
}

std::vector<VertexId> TableLookup(const Graph& graph,
                                  const std::vector<VertexId>& n, VertexId v) {
    std::vector<VertexId> m;
    const auto& neighbors = UpperNeighbors(graph, v);

    std::ranges::set_intersection(n, neighbors, std::back_inserter(m));

    return m;
}

void NewAddCofaces(const Pointcloud& cloud, const Graph& graph, size_t max_dim,
                   const Simplex& tau, const std::vector<VertexId>& n,
                   Weight current_weight, WSimplices& simplices) {
    simplices.push_back({tau, current_weight});

    if (tau.size() - 1 >= max_dim) {
        return;
    }

    for (VertexId v : n) {
        if (v <= tau.back()) {
            continue;
        }

        Simplex sigma = tau;
        sigma.push_back(v);
        std::sort(sigma.begin(), sigma.end());

        Weight new_weight = current_weight;
        for (auto u : tau) {
            new_weight = std::max(new_weight, cloud.EuclideanDistance(u, v));
        }

        auto m = TableLookup(graph, n, v);
        NewAddCofaces(cloud, graph, max_dim, sigma, m, new_weight, simplices);
    }
}

}  // namespace

Filtration::VietorisRipsBuilder::VietorisRipsBuilder(Weight max_radius,
                                                     size_t max_dim)
    : max_radius_(max_radius),
      max_dim_(max_dim) {
}

WSimplices Filtration::VietorisRipsBuilder::Build(
    const Pointcloud& cloud) const {
    WSimplices simplices;
    const auto cloud_size = static_cast<VertexId>(cloud.Size());
    const auto graph = BuildAdjacencyGraph(cloud, max_radius_);

    for (VertexId u = 0; u < cloud_size; ++u) {
        Simplex tau = {u};
        auto n = UpperNeighbors(graph, u);
        NewAddCofaces(cloud, graph, max_dim_, tau, n, 0.0f, simplices);
    }

    std::sort(simplices.begin(), simplices.end());
    return simplices;
}

#ifdef TOPA_USE_TBB
WSimplices Filtration::FullVietorisRipsBuilder::Build(
    const Pointcloud& cloud) const {
    WSimplices simplices;
    const auto cloud_size = static_cast<VertexId>(cloud.Size());

    detail::FlatMatrix<float> dist(cloud_size, cloud_size);
    tbb::parallel_for(tbb::blocked_range<VertexId>(0, cloud_size),
                      [&](const tbb::blocked_range<VertexId>& r) {
                          for (VertexId i = r.begin(); i < r.end(); ++i) {
                              for (VertexId j = i + 1; j < cloud_size; ++j) {
                                  dist[i][j] = cloud.EuclideanDistance(i, j);
                              }
                          }
                      });

    const size_t total = cloud_size + cloud_size * (cloud_size - 1) / 2 +
                         cloud_size * (cloud_size - 1) * (cloud_size - 2) / 6;
    simplices.reserve(total);

    tbb::enumerable_thread_specific<WSimplices> thread_simplices;

    tbb::parallel_for(
        tbb::blocked_range<VertexId>(0, cloud_size),
        [&](const tbb::blocked_range<VertexId>& r) {
            auto& local_simplices = thread_simplices.local();
            for (VertexId i = r.begin(); i < r.end(); ++i) {
                local_simplices.emplace_back(Simplex{i}, 0.0f);
                for (VertexId j = i + 1; j < cloud_size; ++j) {
                    const float d_ij = dist[i][j];
                    local_simplices.emplace_back(Simplex{i, j}, d_ij);
                    for (VertexId k = j + 1; k < cloud_size; ++k) {
                        const float max_d =
                            std::max({d_ij, dist[i][k], dist[j][k]});
                        local_simplices.emplace_back(Simplex{i, j, k}, max_d);
                    }
                }
            }
        });

    for (auto& local : thread_simplices) {
        simplices.insert(simplices.end(), local.begin(), local.end());
    }

    std::sort(simplices.begin(), simplices.end());
    return simplices;
}
#else
WSimplices Filtration::FullVietorisRipsBuilder::Build(
    const Pointcloud& cloud) const {
    WSimplices simplices;
    const auto cloud_size = static_cast<VertexId>(cloud.Size());

    detail::FlatMatrix<float> dist(cloud_size, cloud_size);
    for (VertexId i = 0; i < cloud_size; ++i) {
        for (VertexId j = i + 1; j < cloud_size; ++j) {
            dist[i][j] = cloud.EuclideanDistance(i, j);
        }
    }

    const size_t total = cloud_size + cloud_size * (cloud_size - 1) / 2 +
                         cloud_size * (cloud_size - 1) * (cloud_size - 2) / 6;
    simplices.reserve(total);

    for (VertexId i = 0; i < cloud_size; ++i) {
        simplices.emplace_back(Simplex{i}, 0.0f);
        for (VertexId j = i + 1; j < cloud_size; ++j) {
            const float d_ij = dist[i][j];
            simplices.emplace_back(Simplex{i, j}, d_ij);
            for (VertexId k = j + 1; k < cloud_size; ++k) {
                const float max_d = std::max({d_ij, dist[i][k], dist[j][k]});
                simplices.emplace_back(Simplex{i, j, k}, max_d);
            }
        }
    }

    std::sort(simplices.begin(), simplices.end());
    return simplices;
}
#endif

Filtration Filtration::VietorisRips(Weight max_radius, size_t max_dim) {
    return Filtration(
        std::make_unique<VietorisRipsBuilder>(max_radius, max_dim));
}

Filtration Filtration::FullVietorisRips() {
    return Filtration(std::make_unique<FullVietorisRipsBuilder>());
}

WSimplices Filtration::Build(const Pointcloud& cloud) const {
    return builder_->Build(cloud);
}

Filtration::Filtration(std::unique_ptr<Builder> builder)
    : builder_(std::move(builder)) {
}

};  // namespace topa
