// #include "svd.hpp"

// #include <Spectra/SymEigsSolver.h>
// #include <Spectra/MatOp/SparseSymMatProd.h>
// #include <Spectra/MatOp/SparseSymShiftSolve.h>
// #include <Spectra/SymEigsShiftSolver.h>

// #include <ranges>
// #include <iostream>

// namespace topa {

// EigenMatrix OrientedBoundaryMatrix(const FilteredComplex& complex) {
//     const size_t n = complex.GetSimplices().size();
//     EigenMatrix matrix(n, n);

//     std::vector<Eigen::Triplet<float>> triplets;
//     triplets.reserve(2 * n);
//     for (size_t pos = 0; pos < n; ++pos) {
//         auto facets = complex.GetFacetsPosition(pos);
//         if (facets.empty()) {
//             continue;
//         }
//         float orientation = 1.0f;
//         for (size_t i = facets.size(); i > 0; --i) {
//             size_t facet_pos = facets[i - 1];
//             triplets.emplace_back(facet_pos, pos, orientation);
//             orientation = -orientation;
//         }
//     }
//     matrix.setFromTriplets(triplets.begin(), triplets.end());
//     matrix.makeCompressed();
//     return matrix;
// }

// EigenMatrix CutMatrix(const EigenMatrix& matrix, const FilteredComplex&
// complex,
//                       int order, Position last_pos) {
//     auto get_indices = [&](size_t size) {
//         std::vector<size_t> indices;
//         for (const auto& pos : complex.GetPosesBySize(size)) {
//             if (pos > last_pos) {
//                 break;
//             }
//             indices.emplace_back(pos);
//         }
//         return indices;
//     };

//     const auto sizes = order == 1 ? std::make_pair(1, 2) : std::make_pair(2,
//     3); std::vector<size_t> rows = get_indices(sizes.first);
//     std::vector<size_t> cols = get_indices(sizes.second);

//     EigenMatrix submatrix(rows.size(), cols.size());
//     for (size_t j = 0; j < cols.size(); ++j) {
//         for (EigenMatrix::InnerIterator it(matrix, cols[j]); it; ++it) {
//             auto row_it = std::lower_bound(rows.begin(), rows.end(),
//             it.row()); if (row_it != rows.end() &&
//                 *row_it == static_cast<size_t>(it.row())) {
//                 size_t new_row = row_it - rows.begin();
//                 submatrix.insert(new_row, j) = it.value();
//             }
//         }
//     }
//     return submatrix;
// }

// EigenMatrix Laplacian(const EigenMatrix& matrix, const FilteredComplex&
// complex,
//                       Position last_pos) {
//     EigenMatrix b0 = CutMatrix(matrix, complex, 1, last_pos);
//     EigenMatrix b1 = CutMatrix(matrix, complex, 2, last_pos);

//     EigenMatrix b0t = b0.transpose();
//     EigenMatrix b1t = b1.transpose();

//     EigenMatrix l1 = b0t * b0 + b1 * b1t;
//     return l1;
// }

// MatrixF Spectra(const EigenMatrix& matrix) {
//     const int n = matrix.rows();
//     const int nev = std::min(1, n - 1);
//     const int ncv = std::min(2 * nev + 1, n);

//     using OpType = Spectra::SparseSymShiftSolve<float>;
//     OpType op(matrix);

//     Spectra::SymEigsShiftSolver<OpType> eigs(op, nev, ncv, 0.0f);
//     eigs.init();
//     size_t en = eigs.compute(Spectra::SortRule::LargestMagn, 2000, 1e-7f,
//                              Spectra::SortRule::SmallestMagn);

//     if (en == 0 || eigs.info() != Spectra::CompInfo::Successful) {
//         return {};
//     }

//     VectorF eigenvalues = eigs.eigenvalues();
//     MatrixF eigenvectors = eigs.eigenvectors();

//     std::vector<size_t> zero_indices;
//     for (size_t i = 0; i < en; ++i) {
//         if (std::abs(eigenvalues[i]) < 1e-6f) {
//             zero_indices.push_back(i);
//         }
//     }

//     MatrixF filtered_eigenvectors(n, zero_indices.size());
//     for (size_t j = 0; j < zero_indices.size(); ++j) {
//         filtered_eigenvectors.col(j) = eigenvectors.col(zero_indices[j]);
//     }

//     return filtered_eigenvectors;
// }

// }  // namespace topa
