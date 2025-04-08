#include "filtration.hpp"

// #define TOPA_PARALLEL

#ifdef TOPA_PARALLEL
#include <tbb/parallel_for.h>
#include <tbb/concurrent_map.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_unordered_set.h>
#include <functional>
#include <algorithm>

namespace topa {

namespace {

using ConcurrentGraph = tbb::concurrent_map<VertexId, std::vector<VertexId>>;
using ConcurrentSimplices = tbb::concurrent_vector<WSimplex>;
using ConcurrentSet = tbb::concurrent_unordered_set<std::string>;

ConcurrentGraph BuildAdjacencyGraph(const Pointcloud& cloud,
                                    Weight max_radius) {
    ConcurrentGraph graph;
    const auto cloud_size = static_cast<VertexId>(cloud.Size());

    tbb::parallel_for(
        tbb::blocked_range<size_t>(0, cloud_size),
        [&](const tbb::blocked_range<size_t>& r) {
            for (size_t u = r.begin(); u != r.end(); ++u) {
                for (VertexId v = u + 1; v < cloud_size; ++v) {
                    if (cloud.EuclideanDistance(u, v) <= max_radius) {
                        graph[u].push_back(v);
                    }
                }
            }
        });

    tbb::parallel_for(tbb::blocked_range<size_t>(0, cloud_size),
                      [&](const tbb::blocked_range<size_t>& r) {
                          for (size_t u = r.begin(); u != r.end(); ++u) {
                              std::sort(graph[u].begin(), graph[u].end());
                          }
                      });

    return graph;
}

std::vector<VertexId> UpperNeighbors(const ConcurrentGraph& graph, VertexId u) {
    auto it = graph.find(u);
    return (it != graph.end()) ? it->second : std::vector<VertexId>{};
}

std::vector<VertexId> TableLookup(const ConcurrentGraph& graph,
                                  const std::vector<VertexId>& n, VertexId v) {
    std::vector<VertexId> m;
    const auto& neighbors = UpperNeighbors(graph, v);

    std::set_intersection(n.begin(), n.end(), neighbors.begin(),
                          neighbors.end(), std::back_inserter(m));

    return m;
}

void NewAddCofaces(const Pointcloud& cloud, const ConcurrentGraph& graph,
                   size_t max_dim, const Simplex& tau,
                   const std::vector<VertexId>& n, Weight current_weight,
                   ConcurrentSimplices& simplices, ConcurrentSet& visited) {
    std::string hash;
    for (auto x : tau) {
        hash += std::to_string(x) + ",";
    }

    if (!visited.insert(hash).second) {
        return;
    }

    simplices.push_back({tau, current_weight});

    if (tau.size() - 1 >= max_dim) {
        return;
    }

    tbb::parallel_for(tbb::blocked_range<size_t>(0, n.size()),
                      [&](const tbb::blocked_range<size_t>& r) {
                          for (size_t i = r.begin(); i != r.end(); ++i) {
                              VertexId v = n[i];
                              Simplex sigma = tau;
                              sigma.push_back(v);
                              std::sort(sigma.begin(), sigma.end());

                              Weight new_weight = current_weight;
                              for (auto u : tau) {
                                  new_weight =
                                      std::max(new_weight,
                                               cloud.EuclideanDistance(u, v));
                              }

                              auto m = TableLookup(graph, n, v);
                              NewAddCofaces(cloud, graph, max_dim, sigma, m,
                                            new_weight, simplices, visited);
                          }
                      });
}

}  // namespace

Filtration::WSimplices Filtration::VietorisRipsBuilder::Build(
    const Pointcloud& cloud) const {
    ConcurrentSimplices concurrent_simplices;
    ConcurrentSet visited;

    const auto graph = BuildAdjacencyGraph(cloud, max_radius_);

    tbb::parallel_for(tbb::blocked_range<VertexId>(0, cloud.Size()),
                      [&](const tbb::blocked_range<VertexId>& r) {
                          for (VertexId u = r.begin(); u != r.end(); ++u) {
                              Simplex tau = {u};
                              auto n = UpperNeighbors(graph, u);
                              NewAddCofaces(cloud, graph, max_dim_, tau, n,
                                            0.0f, concurrent_simplices,
                                            visited);
                          }
                      });

    WSimplices simplices(concurrent_simplices.begin(),
                         concurrent_simplices.end());
    std::sort(simplices.begin(), simplices.end());

    return simplices;
}

Filtration::VietorisRipsBuilder::VietorisRipsBuilder(Weight max_radius,
                                                     size_t max_dim)
    : max_radius_(max_radius),
      max_dim_(max_dim) {
}
Filtration Filtration::VietorisRips(Weight max_radius, size_t max_dim) {
    return Filtration(
        std::make_unique<VietorisRipsBuilder>(max_radius, max_dim));
}

Filtration::WSimplices Filtration::Build(const Pointcloud& cloud) const {
    return builder_->Build(cloud);
}

Filtration::Filtration(std::unique_ptr<Builder> builder)
    : builder_(std::move(builder)) {
}

};  // namespace topa

#else

#include <algorithm>
#include <unordered_set>

namespace topa {

namespace {

using Graph = basic_types::DefaultMap<VertexId, std::vector<VertexId>>;

Graph BuildAdjacencyGraph(const Pointcloud& cloud, Weight max_radius) {
    Graph graph;
    const auto cloud_size = static_cast<VertexId>(cloud.Size());

    for (VertexId u = 0; u < cloud_size; ++u) {
        for (VertexId v = u + 1; v < cloud_size; ++v) {
            if (cloud.EuclideanDistance(u, v) <= max_radius) {
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

    std::set_intersection(n.begin(), n.end(), neighbors.begin(),
                          neighbors.end(), std::back_inserter(m));

    return m;
}

void NewAddCofaces(const Pointcloud& cloud, const Graph& graph, size_t max_dim,
                   const Simplex& tau, const std::vector<VertexId>& n,
                   Weight current_weight, Filtration::WSimplices& simplices,
                   std::unordered_set<std::string>& visited) {
    simplices.push_back({tau, current_weight});

    if (tau.size() - 1 >= max_dim) {
        return;
    }

    for (VertexId v : n) {
        Simplex sigma = tau;
        sigma.push_back(v);
        std::sort(sigma.begin(), sigma.end());

        Weight new_weight = current_weight;
        for (auto u : tau) {
            new_weight = std::max(new_weight, cloud.EuclideanDistance(u, v));
        }

        std::string hash;
        for (auto x : sigma) {
            hash += std::to_string(x) + ",";
        }

        if (visited.count(hash) == 0) {
            visited.insert(hash);
            auto m = TableLookup(graph, n, v);
            NewAddCofaces(cloud, graph, max_dim, sigma, m, new_weight,
                          simplices, visited);
        }
    }
}

}  // namespace

Filtration::WSimplices Filtration::VietorisRipsBuilder::Build(
    const Pointcloud& cloud) const {
    WSimplices simplices;
    const auto cloud_size = static_cast<VertexId>(cloud.Size());
    const auto graph = BuildAdjacencyGraph(cloud, max_radius_);
    std::unordered_set<std::string> visited;

    for (VertexId u = 0; u < cloud_size; ++u) {
        Simplex tau = {u};
        std::string hash = std::to_string(u) + ",";
        visited.insert(hash);

        auto n = UpperNeighbors(graph, u);
        NewAddCofaces(cloud, graph, max_dim_, tau, n, 0.0f, simplices, visited);
    }

    std::sort(simplices.begin(), simplices.end());
    return simplices;
}

Filtration::VietorisRipsBuilder::VietorisRipsBuilder(Weight max_radius,
                                                     size_t max_dim)
    : max_radius_(max_radius),
      max_dim_(max_dim) {
}
Filtration Filtration::VietorisRips(Weight max_radius, size_t max_dim) {
    return Filtration(
        std::make_unique<VietorisRipsBuilder>(max_radius, max_dim));
}

Filtration::WSimplices Filtration::Build(const Pointcloud& cloud) const {
    return builder_->Build(cloud);
}

Filtration::Filtration(std::unique_ptr<Builder> builder)
    : builder_(std::move(builder)) {
}

};  // namespace topa

#endif
