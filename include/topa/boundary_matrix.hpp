#ifndef TOPA_BOUNDARY_MATRIX_HPP_
#define TOPA_BOUNDARY_MATRIX_HPP_

#include "filtered_complex.hpp"
#include "matrix.hpp"

namespace topa {

Matrix BoundaryMatrix(const FilteredComplex& complex);
Matrix BoundaryMatrixFiltered(const FilteredComplex& complex,
                              basic_types::DefaultView<Position> positions);
Matrix CoboundaryMatrix(const FilteredComplex& complex);

};  // namespace topa

#endif
