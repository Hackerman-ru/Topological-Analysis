#include "point.h"

#include <format>

Point::Point(Id id, data_t x, data_t y) : Base(id), x(x), y(y) {};

Weight squared_distance(const Point& lhs, const Point& rhs) {
    Point::data_t dx = (lhs.x >= rhs.x ? (lhs.x - rhs.x) : (rhs.x - lhs.x));
    Point::data_t dy = (lhs.y >= rhs.y ? (lhs.y - rhs.y) : (rhs.y - lhs.y));
    return dx * dx + dy * dy;
}

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.id == rhs.id;
}

std::strong_ordering operator<=>(const Point& lhs, const Point& rhs) {
    return lhs.id <=> rhs.id;
}

Weight Point::squared_norm() const {
    return x * x + y * y;
}

std::string Point::to_string() const {
    return std::format("({}, {})", x, y);
}
