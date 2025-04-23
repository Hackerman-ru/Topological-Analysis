#pragma once

#include "models/separator.hpp"

#include <eigen3/Eigen/SVD>

namespace topa::common {

class SVDSeparator final : public models::Separator<SVDSeparator> {
   public:
    static VectorF Separate(MatrixF basis, MatrixF vectors) {
        if (vectors.size() == 0) {
            return {};
        }
        if (basis.size() == 0) {
            vectors.col(0);
        }

        const size_t v_pos = basis.cols();
        basis.conservativeResize(Eigen::NoChange, v_pos + 1);
        const size_t cols = vectors.cols();
        size_t max_idx = 0;
        float max_sv = 0.0f;
        for (size_t i = 0; i < cols; ++i) {
            basis.col(v_pos) = vectors.col(i);
            Eigen::BDCSVD<Eigen::MatrixXf> svd(basis);
            VectorF svs = svd.singularValues();
            float min_sv = *svs.end();
            if (min_sv > max_sv) {
                max_idx = i;
                max_sv = min_sv;
            }
        }

        return vectors.col(max_idx);
    }
};

}  // namespace topa::common
