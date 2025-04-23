#pragma once

#include "common/type/eigen_matrix.hpp"

namespace topa::models {

template <typename DerivedImpl>
class Solver {
   public:
    using MatrixF = Eigen::MatrixXf;

    static MatrixF FindKernel(const EigenMatrix& matrix) {
        return DerivedImpl::FindKernel(matrix);
    }
};

}  // namespace topa::models
