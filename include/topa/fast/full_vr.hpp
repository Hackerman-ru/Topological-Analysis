#pragma once

#include "common/detail/flat_matrix.hpp"

#include "models/pointcloud.hpp"
#include "models/filtration.hpp"

namespace topa::fast {

class FullVR final : public models::Filtration<FullVR> {
   public:
    template <typename... CloudImpl>
    WSimplices Filter(const models::Pointcloud<CloudImpl...>& cloud) const {
        WSimplices wsimplices;
        const auto cloud_size = static_cast<VertexId>(cloud.Size());

        detail::FlatMatrix<Weight> dist(cloud_size, cloud_size);
        for (VertexId i = 0; i < cloud_size; ++i) {
            for (VertexId j = i + 1; j < cloud_size; ++j) {
                dist[i][j] = static_cast<Weight>(cloud.GetDistance(i, j));
            }
        }

        const std::size_t total =
            cloud_size + cloud_size * (cloud_size - 1) / 2 +
            cloud_size * (cloud_size - 1) * (cloud_size - 2) / 6;
        wsimplices.reserve(total);

        for (VertexId i = 0; i < cloud_size; ++i) {
            wsimplices.emplace_back(Simplex{i}, 0.0f);
            for (VertexId j = i + 1; j < cloud_size; ++j) {
                const Weight d_ij = dist[i][j];
                wsimplices.emplace_back(Simplex{i, j}, d_ij);
                for (VertexId k = j + 1; k < cloud_size; ++k) {
                    const Weight max_d =
                        std::max({d_ij, dist[i][k], dist[j][k]});
                    wsimplices.emplace_back(Simplex{i, j, k}, max_d);
                }
            }
        }

        std::ranges::sort(wsimplices);
        return wsimplices;
    }
};

}  // namespace topa::fast
