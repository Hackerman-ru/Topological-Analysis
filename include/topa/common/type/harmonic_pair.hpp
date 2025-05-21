#pragma once

#include "position.hpp"

#include <vector>

namespace topa {

using HarmonicCycle = std::vector<float>;

struct HarmonicPair {
    Position birth;
    HarmonicCycle birth_edges;
    HarmonicCycle birth_vertices;
    Position death;
    HarmonicCycle death_edges;
    HarmonicCycle death_vertices;

    HarmonicPair(Position b, Position d)
        : birth(b),
          death(d) {};

    bool operator==(const HarmonicPair& other) const = default;
    bool operator<(const HarmonicPair& other) const {
        auto lifetime = Lifetime();
        auto other_lifetime = other.Lifetime();
        if (lifetime != other_lifetime) {
            return lifetime > other_lifetime;
        }
        if (birth != other.birth) {
            return birth < other.birth;
        }
        return death < other.death;
    }

   private:
    std::size_t Lifetime() const {
        return death - birth;
    }
};

template <typename R>
concept HarmonicPairRange =
    std::ranges::input_range<R> &&
    std::same_as<std::ranges::range_value_t<R>, HarmonicPair>;

}  // namespace topa
