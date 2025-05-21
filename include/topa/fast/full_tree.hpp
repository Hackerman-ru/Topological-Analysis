#pragma once

#include "topa/models/simplex_tree.hpp"

#include <algorithm>
#include <cassert>

namespace topa::fast {

// SimplexTree, which assumes that the full 2-skeleton
// was added before any const methods
class FullTree final : public models::SimplexTree<FullTree> {
   public:
    explicit FullTree(size_t n_vertices);

    void Add(VertexRange auto&& simplex, Position pos) {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        std::vector<VertexId> s(simplex.begin(), simplex.end());
        switch (s.size()) {
            case 1:
                VertexPos(s[0]) = pos;
                break;
            case 2:
                EdgePos(s[0], s[1]) = pos;
                break;
            case 3:
                TrianglePos(s[0], s[1], s[2]) = pos;
                break;
        }
    }

    void Add(std::initializer_list<VertexId> simplex, Position pos) {
        Add(std::views::all(simplex), pos);
    }

    bool Has(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        return simplex.size() <= 3;
    }

    bool Has(std::initializer_list<VertexId> simplex) const {
        return Has(std::views::all(simplex));
    }

    Position GetPos(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        std::vector<VertexId> s(simplex.begin(), simplex.end());
        switch (s.size()) {
            case 1:
                return VertexPos(s[0]);
            case 2:
                return EdgePos(s[0], s[1]);
            case 3:
                return TrianglePos(s[0], s[1], s[2]);
            default:
                return kUnknownPos;
        }
    }

    Position GetPos(std::initializer_list<VertexId> simplex) const {
        return GetPos(std::views::all(simplex));
    }

    Positions GetFacetsPos(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        std::vector<VertexId> s(simplex.begin(), simplex.end());
        Positions facets;

        switch (s.size()) {
            case 3: {
                facets.reserve(3);
                facets.emplace_back(EdgePos(s[0], s[1]));
                facets.emplace_back(EdgePos(s[0], s[2]));
                facets.emplace_back(EdgePos(s[1], s[2]));
                break;
            }
            case 2: {
                facets.emplace_back(VertexPos(s[0]));
                facets.emplace_back(VertexPos(s[1]));
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
        std::vector<VertexId> s(simplex.begin(), simplex.end());
        Positions cofacets;

        switch (s.size()) {
            case 2: {
                VertexId u = s[0], v = s[1];

                for (VertexId w = 0; w < n_; ++w) {
                    if (w > v) {
                        cofacets.emplace_back(TrianglePos(u, v, w));
                    } else if (w > u && w < v) {
                        cofacets.emplace_back(TrianglePos(u, w, v));
                    } else if (w < u) {
                        cofacets.emplace_back(TrianglePos(w, u, v));
                    }
                }
                break;
            }

            case 1: {
                VertexId v = s[0];
                for (VertexId u = 0; u < n_; ++u) {
                    if (u < v) {
                        cofacets.emplace_back(EdgePos(u, v));
                    } else if (u > v) {
                        cofacets.emplace_back(EdgePos(v, u));
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
    const Position& VertexPos(VertexId u) const;
    const Position& EdgePos(VertexId u, VertexId v) const;
    const Position& TrianglePos(VertexId u, VertexId v, VertexId w) const;

    Position& VertexPos(VertexId u);
    Position& EdgePos(VertexId u, VertexId v);
    Position& TrianglePos(VertexId u, VertexId v, VertexId w);

    size_t EdgeIndex(VertexId u, VertexId v) const;
    size_t TriangleIndex(VertexId u, VertexId v, VertexId w) const;

   private:
    const size_t n_;
    Positions vertex_pos_;
    Positions edge_pos_;
    Positions triangle_pos_;
};

}  // namespace topa::fast
