#ifndef TOPA_BOUNDARY_MATRIX_HPP_
#define TOPA_BOUNDARY_MATRIX_HPP_

#include "complex.hpp"
#include "matrix.hpp"

namespace topa {

Matrix<size_t> BoundaryMatrix(const Complex& complex);
Matrix<size_t> BoundaryMatrixFiltered(
    const Complex& complex, basic_types::DefaultView<Position> positions);
Matrix<size_t> CoboundaryMatrix(const Complex& complex);

};  // namespace topa

#endif
