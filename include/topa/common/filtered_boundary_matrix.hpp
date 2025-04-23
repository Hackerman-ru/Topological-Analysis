#pragma once

#include "models/filtered_complex.hpp"

namespace topa::common {

template <typename MatrixImpl, typename ComplexImpl>
MatrixImpl BoundaryMatrixFiltered(
    const models::FilteredComplex<ComplexImpl>& complex,
    PositionRange auto&& positions) {
    MatrixImpl matrix;
    matrix.Reserve(positions.size());

    for (const auto& pos : positions) {
        auto facets_pos = complex.GetFacetsPosition(pos);
        if (!facets_pos.empty()) {
            matrix.Insert(pos, std::move(facets_pos));
        }
    }
    return matrix;
}

template <typename MatrixImpl, typename ComplexImpl>
MatrixImpl BoundaryMatrixFiltered(
    const models::FilteredComplex<ComplexImpl>& complex,
    std::initializer_list<Position> positions) {
    return BoundaryMatrixFiltered(complex, std::views::all(positions));
}

}  // namespace topa::common
