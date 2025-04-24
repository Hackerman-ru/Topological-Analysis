#pragma once

#include "common/detail/flat_matrix.hpp"

#include "models/pointcloud.hpp"
#include "models/filtration.hpp"

namespace topa::fast {

class FullVR final : public models::Filtration<FullVR> {
   public:
    template <typename... CloudImpl>
    FSimplices Filter(const models::Pointcloud<CloudImpl...>& cloud) const {
        FSimplices fsimplices;
        const auto cloud_size = static_cast<VertexId>(cloud.Size());

        detail::FlatMatrix<FiltrationValue> dist(cloud_size, cloud_size);
        for (VertexId i = 0; i < cloud_size; ++i) {
            for (VertexId j = i + 1; j < cloud_size; ++j) {
                dist[i][j] =
                    static_cast<FiltrationValue>(cloud.GetDistance(i, j));
            }
        }

        const std::size_t total =
            cloud_size + cloud_size * (cloud_size - 1) / 2 +
            cloud_size * (cloud_size - 1) * (cloud_size - 2) / 6;
        fsimplices.reserve(total);

        for (VertexId i = 0; i < cloud_size; ++i) {
            fsimplices.emplace_back(Simplex{i}, 0.0f);
            for (VertexId j = i + 1; j < cloud_size; ++j) {
                const FiltrationValue d_ij = dist[i][j];
                fsimplices.emplace_back(Simplex{i, j}, d_ij);
                for (VertexId k = j + 1; k < cloud_size; ++k) {
                    const FiltrationValue max_d =
                        std::max({d_ij, dist[i][k], dist[j][k]});
                    fsimplices.emplace_back(Simplex{i, j, k}, max_d);
                }
            }
        }

        std::ranges::sort(fsimplices);
        return fsimplices;
    }
};

}  // namespace topa::fast
