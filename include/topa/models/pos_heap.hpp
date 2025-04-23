#pragma once

#include "common/type/position.hpp"

#include <vector>

namespace topa::models {

template <typename DerivedImpl>
class PosHeap {
   public:
    using Positions = std::vector<Position>;

    Position GetMaxPos() const {
        return static_cast<const DerivedImpl*>(this)->GetMaxPos();
    }

    bool IsEmpty() const {
        return static_cast<const DerivedImpl*>(this)->IsEmpty();
    }

    template <PositionRange R>
    void Add(R&& range) {
        static_cast<DerivedImpl*>(this)->Add(std::forward<R>(range));
    }

    void Add(std::initializer_list<Position> positions) {
        Add(std::views::all(positions));
    }

    Position PopMaxPos() {
        return static_cast<DerivedImpl*>(this)->PopMaxPos();
    }

    // Assuming ascending order in the output
    Positions PopAll() {
        return static_cast<DerivedImpl*>(this)->PopAll();
    }

    void Clear() {
        static_cast<DerivedImpl*>(this)->Clear();
    }
};

}  // namespace topa::models
