#ifndef __POINT_H__
#define __POINT_H__

#include "base.h"

#include <type_traits>

template<size_t dimensions>
class Point : public Base {
public:
    using data_t = int64_t;
    using Coordinates = std::array<data_t, dimensions>;

    static_assert(std::is_convertible_v<data_t, Weight>, "Not weightable");

    Point(Id id, Coordinates coordinates) : Base(id), m_coordinates(coordinates) {};

    friend Weight squared_distance(const Point& lhs, const Point& rhs) {
        Weight result = 0;

        for (size_t i = 0; i < dimensions; ++i) {
            data_t distance = std::abs(lhs.m_coordinates[i] - rhs.m_coordinates[i]);
            result += distance * distance;
        }

        return result;
    }

    friend bool operator==(const Point& lhs, const Point& rhs) {
        return lhs.m_id == rhs.m_id;
    }

    friend std::strong_ordering operator<=>(const Point& lhs, const Point& rhs) {
        return lhs.m_id <=> rhs.m_id;
    }

    Weight squared_norm() const {
        Weight result = 0;

        for (const auto& coordinate : m_coordinates) {
            result += coordinate * coordinate;
        }

        return result;
    }

    std::string to_string() const override {
        std::string result = "(";

        for (const auto& coordinate : m_coordinates) {
            result += std::to_string(coordinate) + ", ";
        }

        if (dimensions > 0) {
            result.pop_back();
            result.pop_back();
        }

        result += ")";
        return result;
    }

private:
    Coordinates m_coordinates;
};

#endif
