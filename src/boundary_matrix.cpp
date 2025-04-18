#include "boundary_matrix.hpp"

namespace topa {

Matrix BoundaryMatrix(const FilteredComplex& complex) {
    Matrix matrix;
    const auto size = complex.GetSimplices().size();
    matrix.reserve(size);

    for (size_t pos = 0; pos < size; ++pos) {
        auto facets = complex.GetFacetsPosition(pos);
        if (!facets.empty()) {
            matrix[pos] = std::move(facets);
        }
    }
    return matrix;
}

Matrix CoboundaryMatrix(const FilteredComplex& complex) {
    Matrix matrix;
    const auto size = complex.GetSimplices().size();
    matrix.reserve(size);

    for (size_t pos = 0; pos < size; ++pos) {
        const size_t reversed_pos = size - 1 - pos;
        auto cofacets = complex.GetCofacetsPosition(pos);
        if (cofacets.empty()) {
            continue;
        }
        for (auto& cofacet : cofacets) {
            cofacet = size - 1 - cofacet;
        }
        matrix[reversed_pos] = std::move(cofacets);
    }
    return matrix;
}

Matrix BoundaryMatrixFiltered(const FilteredComplex& complex,
                              basic_types::DefaultView<Position> positions) {
    Matrix matrix;
    const auto size = complex.GetSimplices().size();
    matrix.reserve(size);

    for (const auto pos : positions) {
        auto facets = complex.GetFacetsPosition(pos);
        if (!facets.empty()) {
            matrix[pos] = std::move(facets);
        }
    }
    return matrix;
}

};  // namespace topa
