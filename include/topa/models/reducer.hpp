#pragma once

#include "common/type/low.hpp"

#include <vector>

namespace topa::models {

template <typename DerivedImpl>
class Reducer {
   public:
    using Lows = std::vector<Low>;

    template <typename Matrix>
    Lows Reduce(Matrix&& matrix) const {
        return DerivedImpl::Reduce(std::forward<Matrix>(matrix));
    }
};

}  // namespace topa::models
