#ifndef TOPA_SIMPLEX_HPP_
#define TOPA_SIMPLEX_HPP_

#include "basic_types.hpp"

#include <limits>

namespace topa {

using VertexId = basic_types::DefaultId;
using Simplex = basic_types::DefaultContainer<VertexId>;
using Weight = basic_types::DefaultFloat;
static constexpr Weight kEpsilon = std::numeric_limits<Weight>::epsilon() * 100;

struct WSimplex {
    Simplex simplex;
    Weight weight;

    bool operator==(const WSimplex& other) const {
        const Weight diff = weight - other.weight;
        const Weight max_val =
            std::max(std::abs(weight), std::abs(other.weight));

        if (std::abs(diff) > kEpsilon * max_val) {
            return false;
        }

        return simplex == other.simplex;
    }

    bool operator<(const WSimplex& other) const {
        const Weight diff = weight - other.weight;
        const Weight max_val =
            std::max(std::abs(weight), std::abs(other.weight));

        if (std::abs(diff) > kEpsilon * max_val) {
            return weight < other.weight;
        }

        if (simplex.size() != other.simplex.size()) {
            return simplex.size() < other.simplex.size();
        }
        return simplex < other.simplex;
    }
};

};  // namespace topa

#endif
