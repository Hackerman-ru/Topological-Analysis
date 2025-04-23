#pragma once

#include "models/simplex_tree.hpp"

#include <algorithm>
#include <cassert>

namespace topa::fast {

// SimplexTree, which assumes that the full 2-skeleton
// was added before any const methods
class FullTreeOpt final : public models::SimplexTree<FullTreeOpt> {
   public:
    explicit FullTreeOpt(size_t n_vertices);

    void Add(VertexRange auto&& simplex, Position pos) {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        auto it = simplex.begin();
        auto end = simplex.end();
        if (it == end) {
            return;
        }

        VertexId v0 = *it++;
        if (it == end) {
            VertexPos(v0) = pos;
            return;
        }

        VertexId v1 = *it++;
        if (it == end) {
            EdgePos(v0, v1) = pos;
            return;
        }

        VertexId v2 = *it++;
        if (it == end) {
            TrianglePos(v0, v1, v2) = pos;
            return;
        }

        assert(false && "Simplex size >3");
    }

    void Add(std::initializer_list<VertexId> simplex, Position pos) {
        Add(std::views::all(simplex), pos);
    }

    bool Has(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        auto it = simplex.begin();
        auto end = simplex.end();
        size_t count = 0;
        while (it != end && count <= 3) {
            ++it;
            ++count;
        }
        return count <= 3;
    }

    bool Has(std::initializer_list<VertexId> simplex) const {
        return Has(std::views::all(simplex));
    }

    Position GetPos(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        auto it = simplex.begin();
        auto end = simplex.end();
        if (it == end) {
            return kUnknownPos;
        }

        VertexId v0 = *it++;
        if (it == end) {
            return VertexPos(v0);
        }

        VertexId v1 = *it++;
        if (it == end) {
            return EdgePos(v0, v1);
        }

        VertexId v2 = *it++;
        if (it == end) {
            return TrianglePos(v0, v1, v2);
        }

        return kUnknownPos;
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
        // Positions facets;

        // auto it = simplex.begin();
        // auto end = simplex.end();
        // if (it == end) {
        //     return facets;
        // }

        // VertexId v0 = *it++;
        // if (it == end) {
        //     return facets;
        // }

        // VertexId v1 = *it++;
        // if (it == end) {
        //     facets.emplace_back(VertexPos(v0));
        //     facets.emplace_back(VertexPos(v1));
        //     return facets;
        // }

        // VertexId v2 = *it++;
        // if (it == end) {
        //     facets.reserve(3);
        //     facets.emplace_back(EdgePos(v0, v1));
        //     facets.emplace_back(EdgePos(v0, v2));
        //     facets.emplace_back(EdgePos(v1, v2));
        //     return facets;
        // }

        // assert(false && "Simplex size >3");
        // return facets;
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
        // Positions cofacets;

        // auto it = simplex.begin();
        // auto end = simplex.end();
        // if (it == end) {
        //     return cofacets;
        // }

        // VertexId u = *it++;
        // if (it == end) {
        //     for (VertexId v = 0; v < n_; ++v) {
        //         if (v < u) {
        //             cofacets.emplace_back(EdgePos(v, u));
        //         } else if (v > u) {
        //             cofacets.emplace_back(EdgePos(u, v));
        //         }
        //     }
        //     return cofacets;
        // }

        // VertexId v = *it++;
        // if (it == end) {
        //     for (VertexId w = 0; w < n_; ++w) {
        //         if (w < u) {
        //             cofacets.emplace_back(TrianglePos(w, u, v));
        //         } else if (w > u && w < v) {
        //             cofacets.emplace_back(TrianglePos(u, w, v));
        //         } else if (w > v) {
        //             cofacets.emplace_back(TrianglePos(u, v, w));
        //         }
        //     }
        //     return cofacets;
        // }

        // it++;
        // if (it == end) {
        //     return cofacets;
        // }

        // assert(false && "Simplex size >3");
        // return cofacets;
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
