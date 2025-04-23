#pragma once

#include "models/filtered_complex.hpp"

#include <algorithm>

namespace topa::common {

template <typename MatrixImpl, typename ComplexImpl>
MatrixImpl CoboundaryMatrix(
    const models::FilteredComplex<ComplexImpl>& complex) {
    MatrixImpl matrix;
    const auto size = complex.Size();
    matrix.Reserve(size);

    auto reverse = [size](Position& pos) {
        pos = size - 1 - pos;
    };

    for (size_t pos = 0; pos < size; ++pos) {
        size_t reversed_pos = pos;
        auto cofacets_pos = complex.GetCofacetsPosition(pos);
        if (cofacets_pos.empty()) {
            continue;
        }
        reverse(reversed_pos);
        std::ranges::for_each(cofacets_pos, reverse);
        matrix.Insert(reversed_pos, std::move(cofacets_pos));
    }
    return matrix;
}

}  // namespace topa::common
