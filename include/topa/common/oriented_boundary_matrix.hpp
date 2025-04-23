#pragma once

#include "common/type/eigen_matrix.hpp"

#include "models/filtered_complex.hpp"

namespace topa::common {

template <typename ComplexImpl>
EigenMatrix OrientedBoundaryMatrix(
    const models::FilteredComplex<ComplexImpl>& complex) {
    const std::size_t n = complex.Size();
    EigenMatrix matrix(n, n);

    std::vector<Eigen::Triplet<float>> triplets;
    triplets.reserve(2 * n);
    for (Position pos = 0; pos < n; ++pos) {
        auto facets_pos = complex.GetFacetsPosition(pos);
        if (facets_pos.empty()) {
            continue;
        }
        std::ranges::sort(facets_pos);
        float orientation = 1.0f;
        for (const auto& facet_pos : facets_pos) {
            triplets.emplace_back(facet_pos, pos, orientation);
            orientation = -orientation;
        }
    }
    matrix.setFromTriplets(triplets.begin(), triplets.end());
    return matrix;
}

}  // namespace topa::common
