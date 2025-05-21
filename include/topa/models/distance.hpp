#pragma once

#include "topa/common/type/filtration_value.hpp"

namespace topa::models {

template <typename DerivedImpl>
class Distance {
   public:
    template <typename LeftPoint, typename RightPoint>
    static FiltrationValue GetDistance(LeftPoint&& lhs, RightPoint&& rhs) {
        return DerivedImpl::GetDistance(std::forward<LeftPoint>(lhs),
                                        std::forward<RightPoint>(rhs));
    }
};

}  // namespace topa::models
