#include "boundary_matrix.hpp"

namespace topa {

Matrix<size_t> BoundaryMatrix(const Complex& complex) {
    Matrix<size_t> matrix;
    const auto size = complex.GetSimplices().size();
    matrix.reserve(size);

    for (size_t pos = 0; pos < size; ++pos) {
        matrix.emplace_back(complex.GetFacetsPosition(pos));
    }
    return matrix;
}

Matrix<size_t> CoboundaryMatrix(const Complex& complex) {
    Matrix<size_t> matrix;
    const auto size = complex.GetSimplices().size();
    matrix.reserve(size);

    for (size_t pos = 0; pos < size; ++pos) {
        matrix.emplace_back(complex.GetCofacetsPosition(pos));
    }
    return matrix;
}

Matrix<size_t> BoundaryMatrixFiltered(
    const Complex& complex, basic_types::DefaultView<Position> positions) {
    Matrix<size_t> matrix;
    matrix.resize(complex.GetSimplices().size());

    for (const auto pos : positions) {
        matrix[pos] = complex.GetFacetsPosition(pos);
    }
    return matrix;
}

};  // namespace topa
