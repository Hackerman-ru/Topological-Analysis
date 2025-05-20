#pragma once

#include "topa/common/type/simplex.hpp"
#include "topa/models/pointcloud.hpp"

namespace topa::models {

template <typename DerivedImpl>
class Filtration {
   public:
    using FSimplices = std::vector<FSimplex>;

   public:
    template <typename Cloud>
    FSimplices Filter(Cloud&& cloud) const {
        return static_cast<const DerivedImpl*>(this)->Filter(
            std::forward<Cloud>(cloud));
    }
};

}  // namespace topa::models
