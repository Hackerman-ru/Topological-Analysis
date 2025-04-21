#pragma once

#include "vertex.hpp"
#include "weight.hpp"

#include <vector>
#include <limits>
#include <cassert>

namespace topa::common {

using Simplex = std::vector<VertexId>;

class WSimplex {
   public:
    WSimplex(Simplex simplex, Weight weight)
        : simplex_(std::move(simplex)),
          weight_(std::move(weight)) {
        assert(std::is_sorted(simplex_.begin(), simplex_.end()));
    }

    const Simplex& GetSimplex() const {
        return simplex_;
    }

    const Weight& GetWeight() const {
        return weight_;
    }

    size_t Size() const {
        return simplex_.size();
    }

    size_t Dim() const {
        return simplex_.size() - 1;
    }

    bool operator==(const WSimplex& other) const {
        if (weight_ != other.weight_) {
            return false;
        }

        return simplex_ == other.simplex_;
    }

    bool operator<(const WSimplex& other) const {
        if (weight_ != other.weight_) {
            return weight_ < other.weight_;
        }

        if (simplex_.size() != other.simplex_.size()) {
            return simplex_.size() < other.simplex_.size();
        }

        return simplex_ < other.simplex_;
    }

   private:
    Simplex simplex_;
    Weight weight_;
};

}  // namespace topa::common
