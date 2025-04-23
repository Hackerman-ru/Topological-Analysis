#pragma once

#include "common/type/position.hpp"
#include "common/type/vertex.hpp"

#include <vector>

namespace topa::models {

template <typename DerivedImpl>
class SimplexTree {
   public:
    using Positions = std::vector<Position>;

    template <VertexRange R>
    void Add(R&& simplex, Position pos) {
        static_cast<DerivedImpl*>(this)->Add(std::forward<R>(simplex), pos);
    }

    void Add(std::initializer_list<VertexId> simplex, Position pos) {
        Add(std::views::all(simplex), pos);
    }

    template <VertexRange R>
    bool Has(R&& simplex) const {
        return static_cast<const DerivedImpl*>(this)->Has(
            std::forward<R>(simplex));
    }

    bool Has(std::initializer_list<VertexId> simplex) const {
        return Has(std::views::all(simplex));
    }

    template <VertexRange R>
    Position GetPos(R&& simplex) const {
        return static_cast<const DerivedImpl*>(this)->GetPos(
            std::forward<R>(simplex));
    }

    Position GetPos(std::initializer_list<VertexId> simplex) const {
        return GetPos(std::views::all(simplex));
    }

    template <VertexRange R>
    Positions GetFacetsPos(R&& simplex) const {
        return static_cast<const DerivedImpl*>(this)->GetFacetsPos(
            std::forward<R>(simplex));
    }

    Positions GetFacetsPos(std::initializer_list<VertexId> simplex) const {
        return GetFacetsPos(std::views::all(simplex));
    }

    template <VertexRange R>
    Positions GetCofacetsPos(R&& simplex) const {
        return static_cast<const DerivedImpl*>(this)->GetCofacetsPos(
            std::forward<R>(simplex));
    }

    Positions GetCofacetsPos(std::initializer_list<VertexId> simplex) const {
        return GetCofacetsPos(std::views::all(simplex));
    }
};

}  // namespace topa::models
