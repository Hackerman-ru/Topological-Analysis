#ifndef TOPA_SVD_HPP_
#define TOPA_SVD_HPP_

#include "filtered_complex.hpp"

#include <eigen3/Eigen/SparseCore>

namespace topa {

using SingularValues = basic_types::DefaultContainer<basic_types::DefaultFloat>;
using EigenMatrix = Eigen::SparseMatrix<float>;
using VectorF = Eigen::VectorXf;
using MatrixF = Eigen::MatrixXf;

EigenMatrix OrientedBoundaryMatrix(const FilteredComplex& complex);
EigenMatrix CutMatrix(const EigenMatrix& matrix, const FilteredComplex& complex,
                      int order, Position last_pos);
EigenMatrix Laplacian(const EigenMatrix& matrix, const FilteredComplex& complex,
                      Position last_pos);
MatrixF Spectra(const EigenMatrix& matrix);

}  // namespace topa

#endif
