#pragma once

#include "models/pointcloud.hpp"
#include "models/filtration.hpp"
#include "models/filtered_complex.hpp"
#include "models/simplex_tree.hpp"
#include "common/type/simplex.hpp"

#include <iostream>

namespace topa::common {

template <typename TreeImpl>
class FilteredComplex final
    : public models::FilteredComplex<FilteredComplex<TreeImpl>> {
   public:
    using Positions = typename topa::models::FilteredComplex<
        topa::common::FilteredComplex<TreeImpl>>::Positions;
    using DimPoses = std::vector<Positions>;
    using WSimplices = std::vector<WSimplex>;
    using Tree = TreeImpl;

   public:
    template <typename... CloudImpl, typename FiltrationImpl>
    static FilteredComplex From(
        const models::Pointcloud<CloudImpl...>& cloud,
        const models::Filtration<FiltrationImpl>& filtration) {
        WSimplices wsimplices = filtration.Filter(cloud);
        return FilteredComplex(std::move(wsimplices), cloud.Size());
    }

    FilteredComplex(WSimplices wsimplices, std::size_t n_vertices)
        : wsimplices_(std::move(wsimplices)),
          tree_(n_vertices) {
        assert(std::is_sorted(wsimplices_.begin(), wsimplices_.end()));
        for (Position pos = 0; pos < wsimplices_.size(); ++pos) {
            const auto& simplex = wsimplices_[pos].GetSimplex();
            tree_.Add(simplex, pos);
            std::size_t dim = simplex.size() - 1;
            if (dim_poses_.size() <= dim) {
                dim_poses_.resize(dim + 1);
            }
            dim_poses_[dim].emplace_back(pos);
        }
    }

    std::size_t Size() const {
        return wsimplices_.size();
    }

    const WSimplices& GetWSimplices() const {
        return wsimplices_;
    }

    std::size_t GetSizeByPos(Position pos) const {
        return wsimplices_[pos].Size();
    }

    const Positions& GetPosesBySize(std::size_t size) const {
        assert(dim_poses_.size() >= size);
        return dim_poses_.at(size - 1);
    }

    Positions GetFacetsPosition(Position pos) const {
        const auto& simplex = wsimplices_[pos].GetSimplex();
        return tree_.GetFacetsPos(simplex);
    }

    Positions GetCofacetsPosition(Position pos) const {
        const auto& simplex = wsimplices_[pos].GetSimplex();
        return tree_.GetCofacetsPos(simplex);
    }

   private:
    WSimplices wsimplices_;
    DimPoses dim_poses_;
    Tree tree_;
};

}  // namespace topa::common
