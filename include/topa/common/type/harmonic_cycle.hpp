#pragma once

#include "position.hpp"

#include <vector>

namespace topa {

struct HarmonicCycle {
    using Cycle = std::vector<float>;

    Position birth;
    Cycle birth_edges;
    Cycle birth_vertices;
    Position death;
    Cycle death_edges;
    Cycle death_vertices;

    HarmonicCycle(Position b, Position d)
        : birth(b),
          death(d) {};

    bool operator==(const HarmonicCycle& other) const = default;
    bool operator<(const HarmonicCycle& other) const {
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
concept HarmonicCycleRange =
    std::ranges::input_range<R> &&
    std::same_as<std::ranges::range_value_t<R>, HarmonicCycle>;

}  // namespace topa
