#pragma once

#include "models/pointcloud.hpp"
#include "models/filtration.hpp"

#include <unordered_map>

namespace topa::common {

class NewVR final : public models::Filtration<NewVR> {
    Weight max_radius_;
    size_t max_dim_;

   public:
    NewVR(Weight max_radius, size_t max_dim)
        : max_radius_(max_radius),
          max_dim_(max_dim) {
    }

    template <typename... CloudImpl>
    WSimplices Filter(const models::Pointcloud<CloudImpl...>& cloud) const {
        return Wrapper<CloudImpl...>::Filter(cloud, max_radius_, max_dim_);
    }

   private:
    template <typename... CloudImpl>
    class Wrapper {
        using Cloud = models::Pointcloud<CloudImpl...>;
        using Graph = std::unordered_map<VertexId, std::vector<VertexId>>;

       public:
        static WSimplices Filter(const Cloud& cloud, Weight max_radius,
                                 size_t max_dim) {
            WSimplices simplices;
            const auto cloud_size = static_cast<VertexId>(cloud.Size());
            const auto graph = BuildAdjacencyGraph(cloud, max_radius);

            for (VertexId u = 0; u < cloud_size; ++u) {
                Simplex tau = {u};
                auto n = UpperNeighbors(graph, u);
                NewAddCofaces(cloud, graph, max_dim, tau, n, 0.0f, simplices);
            }

            std::sort(simplices.begin(), simplices.end());
            return simplices;
        }

       private:
        static Graph BuildAdjacencyGraph(const Cloud& cloud,
                                         Weight max_radius) {
            Graph graph;
            const auto cloud_size = static_cast<VertexId>(cloud.Size());

            for (VertexId u = 0; u < cloud_size; ++u) {
                for (VertexId v = u + 1; v < cloud_size; ++v) {
                    if (static_cast<Weight>(cloud.GetDistance(u, v)) <=
                        max_radius) {
                        graph[u].emplace_back(v);
                    }
                }
            }

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
                                  Weight current_weight,
                                  WSimplices& simplices) {
            simplices.emplace_back(tau, current_weight);

            if (tau.size() >= max_dim + 1) {
                return;
            }

            for (VertexId v : n) {
                assert(v > tau.back());
                Simplex sigma = tau;
                sigma.emplace_back(v);
                std::ranges::sort(sigma);

                Weight new_weight = current_weight;
                for (auto u : tau) {
                    new_weight =
                        std::max(new_weight,
                                 static_cast<Weight>(cloud.GetDistance(u, v)));
                }

                auto m = TableLookup(graph, n, v);
                NewAddCofaces(cloud, graph, max_dim, sigma, m, new_weight,
                              simplices);
            }
        }
    };
};

}  // namespace topa::common
