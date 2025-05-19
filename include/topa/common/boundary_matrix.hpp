#pragma once

#include "models/filtered_complex.hpp"

namespace topa::common {

template <typename Matrix, typename ComplexImpl>
Matrix BoundaryMatrix(const models::FilteredComplex<ComplexImpl>& complex) {
    Matrix matrix;
    const auto size = complex.Size();
    matrix.Reserve(size);

    for (Position pos = 0; pos < size; ++pos) {
        auto facets_pos = complex.GetFacetsPosition(pos);
        if (!facets_pos.empty()) {
            matrix.Insert(pos, std::move(facets_pos));
        }
    }
    return matrix;
}

};  // namespace topa::common
