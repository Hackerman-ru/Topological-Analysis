#pragma once

#include <utility>

namespace topa::models {

template <typename DerivedImpl, typename Traits>
class Pointcloud {
   public:
    using Distance = typename Traits::Distance;
    using Point = typename Traits::Point;
    using Points = typename Traits::Points;

   public:
    void Add(Point point) {
        static_cast<DerivedImpl*>(this)->Add(std::move(point));
    }
    std::size_t Size() const {
        return static_cast<const DerivedImpl*>(this)->Size();
    }
    const Points& GetPoints() const {
        return static_cast<const DerivedImpl*>(this)->GetPoints();
    }
};

}  // namespace topa::models
