#pragma once

#include "topa/common/type/low.hpp"
#include "topa/common/type/persistence_pair.hpp"

#include <vector>

namespace topa::detail {

std::vector<PersistencePair> LowToPairs(LowRange auto&& lows) {
    std::vector<PersistencePair> pairs;
    for (size_t death = 0; death < lows.size(); ++death) {
        Low birth = lows[death];
        if (birth != kUnknownLow) {
            pairs.emplace_back(PersistencePair{birth, death});
        }
    }
    return pairs;
}

}  // namespace topa::detail
