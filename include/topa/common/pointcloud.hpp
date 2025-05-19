#pragma once

#include "common/detail/flat_matrix.hpp"
#include "models/pointcloud.hpp"

#include <optional>
#include <string>

namespace topa::common {

struct PointcloudTraits {
    using CoordinateType = float;
    using Distance = CoordinateType;
    using Point = std::vector<CoordinateType>;
    using Points = detail::FlatMatrix<CoordinateType>;
};

class Pointcloud final
    : public models::Pointcloud<Pointcloud, PointcloudTraits> {
   public:
    using CoordinateType = PointcloudTraits::CoordinateType;

    static std::optional<Pointcloud> Load(std::string file_path);

    explicit Pointcloud(size_t point_dim);
    void Add(Point point);
    size_t Size() const;
    const Points& GetPoints() const;
    void Clear();

   private:
    size_t dim_;
    Points points_;
};

}  // namespace topa::common
