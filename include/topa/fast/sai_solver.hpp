#pragma once

#include "models/solver.hpp"

#include <Spectra/SymEigsSolver.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/SymEigsShiftSolver.h>

namespace topa::fast {

template <std::size_t nev = 1, std::size_t ncv = nev * 2,
          std::size_t iterations = 500, std::size_t precision = 7>
class ShiftSolver final
    : public models::Solver<ShiftSolver<nev, ncv, iterations, precision>> {
    using VectorF = Eigen::VectorXf;
    using MatrixF = Eigen::MatrixXf;

   public:
    static MatrixF FindKernel(const EigenMatrix& matrix) {
        static constexpr float sigma = 1e-6f;
        const int n = matrix.rows();
        const float tol = std::pow(1e-1, precision);

        using OpType = Spectra::SparseSymShiftSolve<float>;
        OpType op(matrix);

        Spectra::SymEigsShiftSolver<OpType> eigs(op, nev, ncv, sigma);
        eigs.init();
        size_t en = 0;
        try {
            en = eigs.compute(Spectra::SortRule::LargestMagn, iterations, tol,
                              Spectra::SortRule::SmallestMagn);
        } catch (...) {
            // Some error, skipping
            return {};
        }

        if (en == 0 || eigs.info() != Spectra::CompInfo::Successful) {
            return {};
        }

        VectorF eigenvalues = eigs.eigenvalues();
        MatrixF eigenvectors = eigs.eigenvectors();

        std::vector<size_t> zero_indices;
        for (size_t i = 0; i < en; ++i) {
            if (std::abs(eigenvalues[i]) < tol) {
                zero_indices.push_back(i);
            }
        }

        MatrixF filtered_eigenvectors(n, zero_indices.size());
        for (size_t j = 0; j < zero_indices.size(); ++j) {
            filtered_eigenvectors.col(j) = eigenvectors.col(zero_indices[j]);
        }

        return filtered_eigenvectors;
    }
};

}  // namespace topa::fast
