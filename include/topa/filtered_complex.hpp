#ifndef TOPA_FILTERED_COMPLEX_HPP_
#define TOPA_FILTERED_COMPLEX_HPP_

#include "filtration.hpp"
#include "simplex.hpp"
#include "simplex_tree.hpp"

namespace topa {

class FilteredComplex {
   public:
    using SortedWSimplices = WSimplices;
    using DimPoses = basic_types::DefaultContainer<Positions>;

   public:
    static FilteredComplex From(const Pointcloud& cloud,
                                const Filtration& filtration);
    FilteredComplex(SortedWSimplices wsimplices, size_t vertices_number);

    const WSimplices& GetSimplices() const;
    size_t Size() const;
    size_t GetSizeByPos(Position pos) const;
    const Positions& GetPosesBySize(size_t size) const;
    Positions GetFacetsPosition(Position pos) const;
    Positions GetCofacetsPosition(Position pos) const;

   private:
    FilteredComplex(WSimplices wsimplices, SimplexTree tree);

    void CountDimPoses();

   private:
    DimPoses dim_poses_ = DimPoses(3);
    WSimplices wsimplices_;
    SimplexTree tree_;
};

}  // namespace topa

#endif
