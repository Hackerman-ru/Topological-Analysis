#pragma once

#include <eigen3/Eigen/Core>

namespace topa::models {

template <typename DerivedImpl>
class Separator {
   public:
    using VectorF = Eigen::VectorXf;
    using MatrixF = Eigen::MatrixXf;

   public:
    static VectorF Separate(MatrixF basis, MatrixF vectors) {
        return DerivedImpl::Separate(std::move(basis), std::move(vectors));
    }
};

}  // namespace topa::models
