#pragma once

#include "topa/models/filtered_complex.hpp"

namespace topa::common {

template <typename Matrix, typename ComplexImpl>
Matrix BoundaryMatrixFiltered(
    const models::FilteredComplex<ComplexImpl>& complex,
    PositionRange auto&& positions) {
    Matrix matrix;
    matrix.Reserve(positions.size());

    for (const auto& pos : positions) {
        auto facets_pos = complex.GetFacetsPosition(pos);
        if (!facets_pos.empty()) {
            matrix.Insert(pos, std::move(facets_pos));
        }
    }
    return matrix;
}

template <typename Matrix, typename ComplexImpl>
Matrix BoundaryMatrixFiltered(
    const models::FilteredComplex<ComplexImpl>& complex,
    std::initializer_list<Position> positions) {
    return BoundaryMatrixFiltered(complex, std::views::all(positions));
}

}  // namespace topa::common
