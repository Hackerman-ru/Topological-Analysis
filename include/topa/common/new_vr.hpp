#pragma once

#include "models/pointcloud.hpp"
#include "models/filtration.hpp"

#include <unordered_map>

namespace topa::common {

class NewVR final : public models::Filtration<NewVR> {
    FiltrationValue max_radius_;
    size_t max_dim_;

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

       public:
        static FSimplices Filter(const Cloud& cloud, FiltrationValue max_radius,
                                 size_t max_dim) {
            FSimplices fsimplices;
            const auto cloud_size = static_cast<VertexId>(cloud.Size());
            const auto graph = BuildAdjacencyGraph(cloud, max_radius);

            for (VertexId u = 0; u < cloud_size; ++u) {
                Simplex tau = {u};
                auto n = UpperNeighbors(graph, u);
                NewAddCofaces(cloud, graph, max_dim, tau, n, 0.0f, fsimplices);
            }

            std::ranges::sort(fsimplices);
            return fsimplices;
        }

       private:
        static Graph BuildAdjacencyGraph(const Cloud& cloud,
                                         FiltrationValue max_radius) {
            Graph graph;
            const auto cloud_size = static_cast<VertexId>(cloud.Size());

            for (VertexId u = 0; u < cloud_size; ++u) {
                for (VertexId v = u + 1; v < cloud_size; ++v) {
                    if (static_cast<FiltrationValue>(cloud.GetDistance(u, v)) <=
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
                                  FiltrationValue current_filtration_value,
                                  FSimplices& fsimplices) {
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
                        static_cast<FiltrationValue>(cloud.GetDistance(u, v)));
                }

                auto m = TableLookup(graph, n, v);
                NewAddCofaces(cloud, graph, max_dim, sigma, m,
                              new_filtration_value, fsimplices);
            }
        }
    };
};

}  // namespace topa::common
