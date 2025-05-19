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

void Pointcloud::Clear() {
    points_.clear();
}

}  // namespace topa::common
