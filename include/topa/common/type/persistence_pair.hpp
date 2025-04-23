#pragma once

#include "position.hpp"

namespace topa {

struct PersistencePair {
    Position birth;
    Position death;

    bool operator==(const PersistencePair& other) const = default;
    bool operator<(const PersistencePair& other) const {
        if (birth != other.birth) {
            return birth < other.birth;
        }
        return death < other.death;
    }
};

template <typename R>
concept PersistencePairRange =
    std::ranges::input_range<R> &&
    std::convertible_to<std::ranges::range_value_t<R>, PersistencePair>;

}  // namespace topa
