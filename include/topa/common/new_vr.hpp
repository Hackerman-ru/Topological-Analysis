#pragma once

#include "common/detail/executor.hpp"

#include "models/pointcloud.hpp"
#include "models/distance.hpp"
#include "models/filtration.hpp"

#ifdef TOPA_USE_TBB
#include <tbb/combinable.h>
#endif

#include <unordered_map>
#include <numeric>

namespace topa::common {

template <typename DistanceImpl>
class NewVR final : public models::Filtration<NewVR<DistanceImpl>> {
    FiltrationValue max_radius_;
    size_t max_dim_;

    using FSimplices = models::Filtration<NewVR<DistanceImpl>>::FSimplices;

   public:
    NewVR(FiltrationValue max_radius, size_t max_dim)
        : max_radius_(max_radius),
          max_dim_(max_dim) {
    }

    template <typename... CloudImpl>
    FSimplices Filter(const models::Pointcloud<CloudImpl...>& cloud) const {
        return Wrapper<CloudImpl...>::Filter(cloud, max_radius_, max_dim_);
    }

   private:
    template <typename... CloudImpl>
    class Wrapper {
        using Cloud = models::Pointcloud<CloudImpl...>;
        using Graph = std::unordered_map<VertexId, std::vector<VertexId>>;
        using Distance = models::Distance<DistanceImpl>;

       public:
        static FSimplices Filter(const Cloud& cloud, FiltrationValue max_radius,
                                 size_t max_dim) {
            const auto cloud_size = static_cast<VertexId>(cloud.Size());
            std::vector<VertexId> vertices(cloud_size);
            std::iota(vertices.begin(), vertices.end(), 0);
            const auto graph = BuildAdjacencyGraph(cloud, vertices, max_radius);

            FSimplices fsimplices;
#ifdef TOPA_USE_TBB
            tbb::combinable<FSimplices> local_fsimplices;
#endif
            detail::Execute(vertices.begin(), vertices.end(), [&](VertexId u) {
#ifdef TOPA_USE_TBB
                auto& local = local_fsimplices.local();
#else
                auto& local = fsimplices;
#endif
                Simplex tau = {u};
                auto n = UpperNeighbors(graph, u);
                NewAddCofaces(cloud, graph, max_dim, tau, n, 0.0f, local);
            });

#ifdef TOPA_USE_TBB
            local_fsimplices.combine_each([&](const FSimplices& vec) {
                fsimplices.insert(fsimplices.end(), vec.begin(), vec.end());
            });
#endif
            detail::Sort(fsimplices.begin(), fsimplices.end());
            return fsimplices;
        }

       private:
        static Graph BuildAdjacencyGraph(const Cloud& cloud,
                                         const std::vector<VertexId>& vertices,
                                         FiltrationValue max_radius) {
            Graph graph;
            const auto& points = cloud.GetPoints();
            const auto cloud_size = static_cast<VertexId>(points.size());

            graph.reserve(cloud_size);
            for (VertexId u = 0; u < cloud_size; ++u) {
                graph[u];
            }

            detail::Execute(vertices.begin(), vertices.end(), [&](VertexId u) {
                std::vector<VertexId> neighbors;
                for (VertexId v = u + 1; v < cloud_size; ++v) {
                    if (Distance::GetDistance(points[u], points[v]) <=
                        max_radius) {
                        neighbors.emplace_back(v);
                    }
                }
                if (!neighbors.empty()) {
                    graph[u] = std::move(neighbors);
                }
            });

            return graph;
        }

        static std::vector<VertexId> UpperNeighbors(const Graph& graph,
                                                    VertexId u) {
            auto it = graph.find(u);
            return (it != graph.end()) ? it->second : std::vector<VertexId>{};
        }

        static std::vector<VertexId> TableLookup(const Graph& graph,
                                                 const std::vector<VertexId>& n,
                                                 VertexId v) {
            std::vector<VertexId> m;
            const auto& neighbors = UpperNeighbors(graph, v);
            std::ranges::set_intersection(n, neighbors, std::back_inserter(m));
            return m;
        }

        static void NewAddCofaces(const Cloud& cloud, const Graph& graph,
                                  size_t max_dim, const Simplex& tau,
                                  const std::vector<VertexId>& n,
                                  FiltrationValue current_filtration_value,
                                  FSimplices& fsimplices) {
            const auto& points = cloud.GetPoints();
            fsimplices.emplace_back(tau, current_filtration_value);

            if (tau.size() >= max_dim + 1) {
                return;
            }

            for (VertexId v : n) {
                assert(v > tau.back());
                Simplex sigma = tau;
                sigma.emplace_back(v);
                std::ranges::sort(sigma);

                FiltrationValue new_filtration_value = current_filtration_value;
                for (auto u : tau) {
                    new_filtration_value = std::max(
                        new_filtration_value,
                        static_cast<FiltrationValue>(
                            Distance::GetDistance(points[u], points[v])));
                }

                auto m = TableLookup(graph, n, v);
                NewAddCofaces(cloud, graph, max_dim, sigma, m,
                              new_filtration_value, fsimplices);
            }
        }
    };
};

}  // namespace topa::common
