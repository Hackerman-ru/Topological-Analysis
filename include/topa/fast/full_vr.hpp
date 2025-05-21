#pragma once

#include "topa/common/detail/flat_matrix.hpp"
#include "topa/common/detail/executor.hpp"

#include "topa/models/pointcloud.hpp"
#include "topa/models/distance.hpp"
#include "topa/models/filtration.hpp"

#ifdef TOPA_USE_TBB
#include <tbb/combinable.h>
#endif
#include <numeric>

namespace topa::fast {

template <typename DistanceImpl>
class FullVR final : public models::Filtration<FullVR<DistanceImpl>> {
    using Distance = models::Distance<DistanceImpl>;
    using FSimplices = models::Filtration<FullVR<DistanceImpl>>::FSimplices;

   public:
    template <typename... CloudImpl>
    FSimplices Filter(const models::Pointcloud<CloudImpl...>& cloud) const {
        const auto& points = cloud.GetPoints();
        const auto cloud_size = static_cast<VertexId>(points.size());
        detail::FlatMatrix<FiltrationValue> dist(cloud_size, cloud_size);

        std::vector<VertexId> vertices(cloud_size);
        std::iota(vertices.begin(), vertices.end(), 0);

        detail::Execute(vertices.begin(), vertices.end(), [&](VertexId i) {
            for (VertexId j = i + 1; j < cloud_size; ++j) {
                dist[i][j] = Distance::GetDistance(points[i], points[j]);
            }
        });

        FSimplices fsimplices;
#ifdef TOPA_USE_TBB
        tbb::combinable<FSimplices> local_simplices;
#endif

        detail::Execute(vertices.begin(), vertices.end(), [&](VertexId i) {
#ifdef TOPA_USE_TBB
            auto& local = local_simplices.local();
#else
            auto& local = fsimplices;
#endif
            local.emplace_back(Simplex{i}, 0.0f);

            for (VertexId j = i + 1; j < cloud_size; ++j) {
                const FiltrationValue d_ij = dist[i][j];
                local.emplace_back(Simplex{i, j}, d_ij);

                for (VertexId k = j + 1; k < cloud_size; ++k) {
                    const FiltrationValue max_d =
                        std::max({d_ij, dist[i][k], dist[j][k]});
                    local.emplace_back(Simplex{i, j, k}, max_d);
                }
            }
        });

#ifdef TOPA_USE_TBB
        local_simplices.combine_each([&](const FSimplices& vec) {
            fsimplices.insert(fsimplices.end(), vec.begin(), vec.end());
        });
#endif

        detail::Sort(fsimplices.begin(), fsimplices.end());
        return fsimplices;
    }
};

}  // namespace topa::fast
