#ifndef TOPA_POINTCLOUD_HPP_
#define TOPA_POINTCLOUD_HPP_

#include "basic_types.hpp"
#include "detail/flat_matrix.hpp"

#include <optional>
#include <string>

namespace topa {

class Pointcloud {
   public:
    using CoordinateType = basic_types::DefaultFloat;
    using Point = basic_types::DefaultContainer<CoordinateType>;
    using Points = detail::FlatMatrix<CoordinateType>;

   public:
    static std::optional<Pointcloud> Load(std::string file_path);

    explicit Pointcloud(size_t dim);
    void Add(Point point);
    size_t Size() const;
    const Points& GetPoints() const;

    CoordinateType EuclideanDistance(size_t i, size_t j) const;
    CoordinateType SquaredEucledianDistance(size_t i, size_t j) const;

   private:
    size_t dim_;
    Points points_;
};

}  // namespace topa

#endif
