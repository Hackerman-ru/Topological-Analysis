#include "common/set_heap.hpp"

namespace topa::common {

Position SetHeap::GetMaxPos() const {
    if (IsEmpty()) {
        return kUnknownPos;
    }
    return *positions_.rbegin();
}

bool SetHeap::IsEmpty() const {
    return positions_.empty();
}

SetHeap& SetHeap::operator+=(const SetHeap& other) {
    for (const auto& pos : other.positions_) {
        auto it = positions_.find(pos);
        if (it == positions_.end()) {
            positions_.insert(pos);
        } else {
            positions_.erase(it);
        }
    }
    return *this;
}

Position SetHeap::PopMaxPos() {
    if (IsEmpty()) {
        return kUnknownPos;
    }
    auto it = positions_.end();
    --it;
    Position max_pos = *it;
    positions_.erase(it);
    return max_pos;
}

SetHeap::Positions SetHeap::PopAll() {
    Positions poses(positions_.begin(), positions_.end());
    positions_.clear();
    return poses;
}

void SetHeap::Clear() {
    positions_.clear();
}

}  // namespace topa::common
