#include "filtered_complex.hpp"

namespace topa {

FilteredComplex FilteredComplex::From(const Pointcloud& cloud,
                                      const Filtration& filtration) {
    std::vector<WSimplex> wsimplices = filtration.Build(cloud);
    assert(std::is_sorted(wsimplices.begin(), wsimplices.end()));

    SimplexTree tree(cloud.Size());
    for (Position pos = 0; pos < wsimplices.size(); ++pos) {
        tree.Add(wsimplices[pos].simplex, pos);
    }
    return FilteredComplex(std::move(wsimplices), std::move(tree));
}

FilteredComplex::FilteredComplex(SortedWSimplices wsimplices,
                                 size_t vertices_number)
    : wsimplices_(std::move(wsimplices)),
      tree_(vertices_number) {
    assert(std::is_sorted(wsimplices_.begin(), wsimplices_.end()));
    for (Position pos = 0; pos < wsimplices_.size(); ++pos) {
        const auto& simplex = wsimplices_[pos].simplex;
        tree_.Add(simplex, pos);
        size_t dim = simplex.size() - 1;
        dim_poses_[dim].emplace_back(pos);
    }
}

const WSimplices& FilteredComplex::GetSimplices() const {
    return wsimplices_;
}

const Positions& FilteredComplex::GetPosesBySize(size_t size) const {
    return dim_poses_[size - 1];
}

Positions FilteredComplex::GetFacetsPosition(Position pos) const {
    const auto& simplex = wsimplices_[pos].simplex;
    auto facets = tree_.GetFacets(simplex);
    return facets;
}

Positions FilteredComplex::GetCofacetsPosition(Position pos) const {
    const auto& simplex = wsimplices_[pos].simplex;
    auto cofacets = tree_.GetCofacets(simplex);
    return cofacets;
}

FilteredComplex::FilteredComplex(WSimplices wsimplices, SimplexTree tree)
    : wsimplices_(std::move(wsimplices)),
      tree_(std::move(tree)) {
    for (Position pos = 0; pos < wsimplices_.size(); ++pos) {
        const auto& simplex = wsimplices_[pos].simplex;
        size_t dim = simplex.size() - 1;
        dim_poses_[dim].emplace_back(pos);
    }
}

}  // namespace topa
