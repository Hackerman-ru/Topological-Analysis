#include "complex.hpp"

namespace topa {

Complex Complex::From(const Pointcloud& cloud, const Filtration& filtration) {
    std::vector<WSimplex> wsimplices = filtration.Build(cloud);
    assert(std::is_sorted(wsimplices.begin(), wsimplices.end()));

    SimplexTree tree(cloud.Size());
    for (size_t pos = 0; pos < wsimplices.size(); ++pos) {
        tree.Add(wsimplices[pos].simplex, pos);
    }
    return Complex(std::move(wsimplices), std::move(tree));
}

const WSimplices& Complex::GetSimplices() const {
    return wsimplices_;
}

Positions Complex::GetFacetsPosition(Position pos) const {
    const auto& simplex = wsimplices_[pos].simplex;
    if (simplex.size() <= 1) {
        return {};
    }
    // auto facets = tree_.GetFacets(simplex);
    Positions facets;
    const auto d = simplex.size() - 1;
    if (d == 1) {
        facets.reserve(2);
    } else {
        facets.reserve(3);
    }
    for (size_t exclude = 0; exclude <= d; ++exclude) {
        Simplex facet;
        for (size_t i = 0; i <= d; ++i) {
            if (i != exclude) {
                facet.emplace_back(simplex[i]);
            }
        }
        facets.emplace_back(tree_.GetPosition(facet));
    }
    return facets;
}

Positions Complex::GetCofacetsPosition(Position pos) const {
    const auto& simplex = wsimplices_[pos].simplex;
    if (simplex.size() >= 3) {
        return {};
    }
    // auto cofacets = tree_.GetCofacets(simplex);
    Positions cofacets;
    const auto n = wsimplices_.size();
    const auto d = simplex.size() - 1;
    if (d == 0) {
        cofacets.reserve(n - 1);
    } else {
        cofacets.reserve(n - 2);
    }
    basic_types::DefaultUnorderedSet<VertexId> vertices(simplex.begin(),
                                                        simplex.end());
    VertexId vn = tree_.GetVerticesNumber();
    for (VertexId vid = 0; vid < vn; ++vid) {
        if (vertices.find(vid) != vertices.end()) {
            continue;
        }
        Simplex cofacet(simplex);
        cofacet.emplace_back(vid);
        std::sort(cofacet.begin(), cofacet.end());
        cofacets.emplace_back(tree_.GetPosition(cofacet));
    }
    return cofacets;
}

Complex::Complex(WSimplices wsimplices, SimplexTree tree)
    : wsimplices_(std::move(wsimplices)),
      tree_(std::move(tree)) {};

}  // namespace topa
