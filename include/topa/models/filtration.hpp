#pragma once

#include "common/type/simplex.hpp"
#include "models/pointcloud.hpp"

namespace topa::models {

template <typename DerivedImpl>
class Filtration {
   public:
    using WSimplices = std::vector<WSimplex>;

   public:
    template <typename Cloud>
    WSimplices Filter(Cloud&& cloud) const {
        return static_cast<const DerivedImpl*>(this)->Filter(
            std::forward<Cloud>(cloud));
    }
};

}  // namespace topa::models
