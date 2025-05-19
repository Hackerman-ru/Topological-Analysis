#pragma once

#include "models/pointcloud.hpp"
#include "models/filtration.hpp"
#include "models/filtered_complex.hpp"
#include "models/simplex_tree.hpp"
#include "common/type/simplex.hpp"

namespace topa::common {

template <typename TreeImpl>
class FilteredComplex final
    : public models::FilteredComplex<FilteredComplex<TreeImpl>> {
   public:
    using Positions = typename topa::models::FilteredComplex<
        topa::common::FilteredComplex<TreeImpl>>::Positions;
    using SizePoses = std::vector<Positions>;
    using FSimplices = std::vector<FSimplex>;
    using Tree = TreeImpl;

   public:
    template <typename... CloudImpl, typename FiltrationImpl>
    static FilteredComplex From(
        const models::Pointcloud<CloudImpl...>& cloud,
        const models::Filtration<FiltrationImpl>& filtration) {
        FSimplices fsimplices = filtration.Filter(cloud);
        return FilteredComplex(std::move(fsimplices), cloud.Size());
    }

    FilteredComplex(FSimplices fsimplices, std::size_t n_vertices)
        : fsimplices_(std::move(fsimplices)),
          size_poses_(1, Positions()),
          tree_(n_vertices) {
        assert(std::is_sorted(fsimplices_.begin(), fsimplices_.end()));
        for (Position pos = 0; pos < fsimplices_.size(); ++pos) {
            const auto& simplex = fsimplices_[pos].GetSimplex();
            tree_.Add(simplex, pos);
            if (size_poses_.size() <= simplex.size()) {
                size_poses_.resize(simplex.size() + 1);
            }
            size_poses_[simplex.size()].emplace_back(pos);
        }
    }

    std::size_t Size() const {
        return fsimplices_.size();
    }

    const FSimplices& GetFSimplices() const {
        return fsimplices_;
    }

    FiltrationValue GetFiltrationValue(Position pos) const {
        return fsimplices_[pos].GetFiltrationValue();
    }

    std::size_t GetSizeByPos(Position pos) const {
        return fsimplices_[pos].Size();
    }

    const Positions& GetPosesBySize(std::size_t size) const {
        if (size >= size_poses_.size()) {
            return size_poses_[0];
        }
        return size_poses_[size];
    }

    Positions GetFacetsPosition(Position pos) const {
        const auto& simplex = fsimplices_[pos].GetSimplex();
        return tree_.GetFacetsPos(simplex);
    }

    Positions GetCofacetsPosition(Position pos) const {
        const auto& simplex = fsimplices_[pos].GetSimplex();
        return tree_.GetCofacetsPos(simplex);
    }

   private:
    FSimplices fsimplices_;
    SizePoses size_poses_;
    Tree tree_;
};

}  // namespace topa::common
