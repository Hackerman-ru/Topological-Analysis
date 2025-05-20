#pragma once

#include "topa/models/distance.hpp"

#include <cassert>
#include <cmath>

namespace topa::common {

class EucledianDistance : public models::Distance<EucledianDistance> {
   public:
    template <typename Point>
    static FiltrationValue GetDistance(const Point& lhs, const Point& rhs) {
        FiltrationValue sum = 0;
        FiltrationValue c = 0;
        auto lit = lhs.begin();
        auto rit = rhs.begin();
        while (lit != lhs.end() && rit != rhs.end()) {
            FiltrationValue diff = static_cast<FiltrationValue>(*lit - *rit);
            FiltrationValue y = (diff * diff) - c;
            FiltrationValue temp = sum + y;
            FiltrationValue compensation = (temp - sum) - y;
            c = -compensation;
            sum = temp;

            ++lit;
            ++rit;
        }
        assert(lit == lhs.end() && rit == rhs.end());
        return static_cast<FiltrationValue>(std::sqrt(sum));
    }
};

}  // namespace topa::common
