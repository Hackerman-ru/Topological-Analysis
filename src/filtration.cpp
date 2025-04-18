#include "filtration.hpp"

#include <algorithm>
#include <unordered_set>
#ifdef TOPA_USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/enumerable_thread_specific.h>
#endif

namespace topa {

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
