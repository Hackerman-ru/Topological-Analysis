#ifndef __POINT_H__
#define __POINT_H__

#include "base.h"

#include <cstdint>

struct Point : Base {
    using data_t = int64_t;

    data_t x;
    data_t y;

    Point(Id id, data_t x, data_t y);

    friend Weight squared_distance(const Point& lhs, const Point& rhs);
    friend bool operator==(const Point& lhs, const Point& rhs);
    friend std::strong_ordering operator<=>(const Point& lhs, const Point& rhs);

    Weight squared_norm() const;
    std::string to_string() const override;
};

#endif
