#pragma once

#include "models/simplex_tree.hpp"

#include <algorithm>
#include <cassert>

namespace topa::fast {

class FlatTree final : public models::SimplexTree<FlatTree> {
   public:
    explicit FlatTree(size_t n_vertices);

    void Add(VertexRange auto&& simplex, Position pos) {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        if (simplex.begin() == simplex.end()) {
            return;
        }
        GetPosImpl(simplex) = pos;
    }

    void Add(std::initializer_list<VertexId> simplex, Position pos) {
        Add(std::views::all(simplex), pos);
    }

    bool Has(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        if (simplex.begin() == simplex.end()) {
            return false;
        }
        return GetPosImpl(simplex) != kUnknownPos;
    }

    bool Has(std::initializer_list<VertexId> simplex) const {
        return Has(std::views::all(simplex));
    }

    Position GetPos(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        if (simplex.begin() == simplex.end()) {
            return kUnknownPos;
        }
        return GetPosImpl(simplex);
    }

    Position GetPos(std::initializer_list<VertexId> simplex) const {
        return GetPos(std::views::all(simplex));
    }

    Positions GetFacetsPos(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        assert(Has(simplex));
        Positions facets;

        switch (simplex.size()) {
            case 3: {
                if (Position pos = EdgePos(simplex[0], simplex[1]);
                    pos != kUnknownPos) {
                    facets.emplace_back(pos);
                }
                if (Position pos = EdgePos(simplex[0], simplex[2]);
                    pos != kUnknownPos) {
                    facets.emplace_back(pos);
                }
                if (Position pos = EdgePos(simplex[1], simplex[2]);
                    pos != kUnknownPos) {
                    facets.emplace_back(pos);
                }
                break;
            }
            case 2: {
                if (Position pos = VertexPos(simplex[0]); pos != kUnknownPos) {
                    facets.emplace_back(pos);
                }
                if (Position pos = VertexPos(simplex[1]); pos != kUnknownPos) {
                    facets.emplace_back(pos);
                }
                break;
            }
        }

        return facets;
    }

    Positions GetFacetsPos(std::initializer_list<VertexId> simplex) const {
        return GetFacetsPos(std::views::all(simplex));
    }

    Positions GetCofacetsPos(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        assert(Has(simplex));
        Positions cofacets;

        switch (simplex.size()) {
            case 2: {
                for (VertexId w = 0; w < n_; ++w) {
                    if (w > simplex[1]) {
                        if (Position pos =
                                TrianglePos(simplex[0], simplex[1], w);
                            pos != kUnknownPos) {
                            cofacets.emplace_back(pos);
                        }
                    } else if (w > simplex[0] && w < simplex[1]) {
                        if (Position pos =
                                TrianglePos(simplex[0], w, simplex[1]);
                            pos != kUnknownPos) {
                            cofacets.emplace_back(pos);
                        }
                    } else if (w < simplex[0]) {
                        if (Position pos =
                                TrianglePos(w, simplex[0], simplex[1]);
                            pos != kUnknownPos) {
                            cofacets.emplace_back(pos);
                        }
                    }
                }
                break;
            }

            case 1: {
                for (VertexId u = 0; u < n_; ++u) {
                    if (u < simplex[0]) {
                        if (Position pos = EdgePos(u, simplex[0]);
                            pos != kUnknownPos) {
                            cofacets.emplace_back(pos);
                        }
                    } else if (u > simplex[0]) {
                        if (Position pos = EdgePos(simplex[0], u);
                            pos != kUnknownPos) {
                            cofacets.emplace_back(pos);
                        }
                    }
                }
                break;
            }
        }

        return cofacets;
    }

    Positions GetCofacetsPos(std::initializer_list<VertexId> simplex) const {
        return GetCofacetsPos(std::views::all(simplex));
    }

   private:
    Position& GetPosImpl(VertexRange auto&& simplex) {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        switch (simplex.size()) {
            case 1:
                return VertexPos(simplex[0]);
            case 2:
                return EdgePos(simplex[0], simplex[1]);
            default:
                return TrianglePos(simplex[0], simplex[1], simplex[2]);
        }
    }

    const Position& GetPosImpl(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        switch (simplex.size()) {
            case 1:
                return VertexPos(simplex[0]);
            case 2:
                return EdgePos(simplex[0], simplex[1]);
            default:
                return TrianglePos(simplex[0], simplex[1], simplex[2]);
        }
    }

    const Position& VertexPos(VertexId u) const;
    const Position& EdgePos(VertexId u, VertexId v) const;
    const Position& TrianglePos(VertexId u, VertexId v, VertexId w) const;

    Position& VertexPos(VertexId u);
    Position& EdgePos(VertexId u, VertexId v);
    Position& TrianglePos(VertexId u, VertexId v, VertexId w);

    size_t EdgeIndex(VertexId u, VertexId v) const;
    size_t TriangleIndex(VertexId u, VertexId v, VertexId w) const;

   private:
    size_t n_;
    Positions vertex_pos_;
    Positions edge_pos_;
    Positions triangle_pos_;
};

}  // namespace topa::fast
