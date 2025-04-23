#include "common/pointcloud.hpp"

#include <fstream>
#include <sstream>

namespace topa::common {

Pointcloud::Pointcloud(size_t point_dim)
    : dim_(point_dim),
      points_(point_dim) {
}

std::optional<Pointcloud> Pointcloud::Load(std::string file_path) {
    std::optional<Pointcloud> pointcloud;
    std::ifstream file(file_path);
    if (!file) {
        return std::nullopt;
    }
    std::string line;
    std::getline(file, line);
    if (line != "OFF") {
        return std::nullopt;
    }
    std::getline(file, line);  // Skipping number of lines

    while (std::getline(file, line)) {
        std::istringstream ss(line);

        Point point;
        CoordinateType value = 0;
        while (ss >> value) {
            point.emplace_back(std::move(value));
        }
        if (!pointcloud) {
            pointcloud.emplace(point.size());
        }
        (*pointcloud).points_.push_back(std::move(point));
    }
    return pointcloud;
}

void Pointcloud::Add(Point point) {
    assert(point.size() == dim_);
    points_.push_back(std::move(point));
}

size_t Pointcloud::Size() const {
    return points_.size();
}

const Pointcloud::Points& Pointcloud::GetPoints() const {
    return points_;
}

Pointcloud::Distance Pointcloud::GetDistance(size_t i, size_t j) const {
    CoordinateType sum = 0;
    CoordinateType c = 0;
    for (size_t k = 0; k < dim_; ++k) {
        CoordinateType diff = points_[i][k] - points_[j][k];
        CoordinateType y = (diff * diff) - c;
        CoordinateType temp = sum + y;
        CoordinateType compensation = (temp - sum) - y;
        c = -compensation;
        sum = temp;
    }
    return sum;
}

}  // namespace topa::common
