#pragma once

#include "topa/models/pos_heap.hpp"

#include <set>

namespace topa::common {

class SetHeap : public models::PosHeap<SetHeap> {
   public:
    explicit SetHeap(size_t) {
        //
    }

    Position GetMaxPos() const;
    bool IsEmpty() const;

    SetHeap& operator+=(const SetHeap& other);

    template <PositionRange R>
    void Add(R&& range) {
        for (const auto& pos : range) {
            auto it = positions_.find(pos);
            if (it == positions_.end()) {
                positions_.insert(pos);
            } else {
                positions_.erase(it);
            }
        }
    }

    void Add(std::initializer_list<Position> positions) {
        Add(std::views::all(positions));
    }

    Position PopMaxPos();
    Positions PopAll();
    void Clear();

   private:
    std::set<Position> positions_;
};

}  // namespace topa::common
