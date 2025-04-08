#include "pointcloud.hpp"

#include <cmath>
#include <fstream>
#include <sstream>

namespace topa {

Pointcloud::Pointcloud(size_t dim)
    : dim_(dim),
      points_(dim) {
}

std::optional<Pointcloud> Pointcloud::Load(std::string file_path) {
    std::optional<Pointcloud> pointcloud;
    std::ifstream file(file_path);
    if (!file) {
        return pointcloud;
    }
    std::string line;
    std::getline(file, line);
    if (line != "OFF") {
        return pointcloud;
    }
    std::getline(file, line);  // Skipping number of lines

    while (std::getline(file, line)) {
        std::istringstream ss(line);

        Point point;
        CoordinateType value = 0;
        while (ss >> value) {
            point.emplace_back(std::move(value));
        }
        if (!pointcloud.has_value()) {
            pointcloud = Pointcloud(point.size());
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

Pointcloud::CoordinateType Pointcloud::EuclideanDistance(size_t i,
                                                         size_t j) const {
    assert(i < points_.size());
    assert(j < points_.size());
    CoordinateType sum = 0;
    for (size_t k = 0; k < dim_; ++k) {
        CoordinateType diff = points_[i][k] - points_[j][k];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

Pointcloud::CoordinateType Pointcloud::SquaredEucledianDistance(
    size_t i, size_t j) const {
    CoordinateType sum = 0;
    for (size_t k = 0; k < dim_; ++k) {
        CoordinateType diff = points_[i][k] - points_[j][k];
        sum += diff * diff;
    }
    return sum;
}

}  // namespace topa
