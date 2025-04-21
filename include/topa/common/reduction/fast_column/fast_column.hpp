#pragma once

#include "common/type/position.hpp"

namespace topa::common::reduction {

template <typename DerivedImpl>
class FastColumn {
   public:
    Position GetMaxPos() const {
        return static_cast<DerivedImpl*>(this)->GetMaxPos();
    }

    bool IsEmpty() const {
        return static_cast<DerivedImpl*>(this)->IsEmpty();
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

    void Clear() {
        static_cast<DerivedImpl*>(this)->Clear();
    }
};

}  // namespace topa::common::reduction
