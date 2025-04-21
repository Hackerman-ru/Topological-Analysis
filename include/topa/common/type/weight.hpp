#pragma once

#include <limits>
#include <algorithm>

namespace topa::common {

class Weight {
   public:
    using Type = float;

    explicit Weight(Type value)
        : value_(value) {
    }

    bool operator==(const Weight& other) const {
        const Type diff = std::abs(value_ - other.value_);
        const Type max_val = std::max(std::abs(value_), std::abs(other.value_));

        return diff <= kEpsilon * max_val;
    }

    bool operator!=(const Weight& other) const {
        return !(*this == other);
    }

    bool operator<(const Weight& other) const {
        if (operator==(other)) {
            return false;
        }
        return value_ < other.value_;
    }

    bool operator>(const Weight& other) const {
        if (operator==(other)) {
            return false;
        }
        return value_ > other.value_;
    }

    bool operator<=(const Weight& other) const {
        if (operator==(other)) {
            return true;
        }
        return value_ < other.value_;
    }
    bool operator>=(const Weight& other) const {
        if (operator==(other)) {
            return true;
        }
        return value_ > other.value_;
    }

    explicit operator float() const {
        return value_;
    }

   private:
    static constexpr Type kEpsilon = std::numeric_limits<Type>::epsilon();

   private:
    Type value_;
};

}  // namespace topa::common
