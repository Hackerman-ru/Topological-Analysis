#pragma once

#include "topa/models/solver.hpp"

#include <eigen3/Eigen/Eigenvalues>

namespace topa::fast {

template <std::size_t precision = 3>
class SelfAdjointSolver final
    : public models::Solver<SelfAdjointSolver<precision>> {
    using VectorF = Eigen::VectorXf;
    using MatrixF = Eigen::MatrixXf;
    static constexpr float tol = std::pow(1e-1, precision);

   public:
    static MatrixF FindKernel(const EigenMatrix& matrix) {
        const int n = matrix.rows();
        Eigen::SelfAdjointEigenSolver<EigenMatrix> eigs(matrix);
        if (eigs.info() != Eigen::Success) {
            return {};
        }

        VectorF eigenvalues = eigs.eigenvalues();
        MatrixF eigenvectors = eigs.eigenvectors();

        std::vector<size_t> zero_indices;
        for (auto i = 0; i < eigenvalues.size(); ++i) {
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
