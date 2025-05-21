#pragma once

#include "vertex.hpp"
#include "filtration_value.hpp"

#include <algorithm>
#include <vector>
#include <limits>
#include <cassert>

namespace topa {

using Simplex = std::vector<VertexId>;

class FSimplex {
   public:
    FSimplex(Simplex simplex, FiltrationValue filtration_value)
        : simplex_(std::move(simplex)),
          filtration_value_(std::move(filtration_value)) {
        assert(std::ranges::is_sorted(simplex_.begin(), simplex_.end()));
    }

    const Simplex& GetSimplex() const {
        return simplex_;
    }

    const FiltrationValue& GetFiltrationValue() const {
        return filtration_value_;
    }

    size_t Size() const {
        return simplex_.size();
    }

    size_t Dim() const {
        return simplex_.size() - 1;
    }

    bool operator==(const FSimplex& other) const {
        if (filtration_value_ != other.filtration_value_) {
            return false;
        }

        return simplex_ == other.simplex_;
    }

    bool operator!=(const FSimplex& other) const {
        if (filtration_value_ != other.filtration_value_) {
            return true;
        }

        if (simplex_.size() != other.simplex_.size()) {
            return true;
        }

        return simplex_ != other.simplex_;
    }

    bool operator<(const FSimplex& other) const {
        if (filtration_value_ != other.filtration_value_) {
            return filtration_value_ < other.filtration_value_;
        }

        if (simplex_.size() != other.simplex_.size()) {
            return simplex_.size() < other.simplex_.size();
        }

        return simplex_ < other.simplex_;
    }

    bool operator<=(const FSimplex& other) const {
        if (*this == other) {
            return true;
        }
        return *this < other;
    }

    bool operator>(const FSimplex& other) const {
        if (filtration_value_ != other.filtration_value_) {
            return filtration_value_ > other.filtration_value_;
        }

        if (simplex_.size() != other.simplex_.size()) {
            return simplex_.size() > other.simplex_.size();
        }

        return simplex_ > other.simplex_;
    }

    bool operator>=(const FSimplex& other) const {
        if (*this == other) {
            return true;
        }
        return *this > other;
    }

   private:
    Simplex simplex_;
    FiltrationValue filtration_value_;
};

}  // namespace topa
