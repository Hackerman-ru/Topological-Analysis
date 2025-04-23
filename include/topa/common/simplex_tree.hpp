#pragma once

#include "models/simplex_tree.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <map>
#include <cassert>

namespace topa::common {

class SimplexTree final : public models::SimplexTree<SimplexTree> {
   private:
    struct Node;
    using NextMap = std::map<VertexId, Node*>;
    using RootNode = std::vector<Node*>;

    struct Node {
        Node* previous;
        VertexId vertex_id;
        Node* sibling = nullptr;
        Position pos = kUnknownPos;
        NextMap next = NextMap();
    };

   public:
    enum class Hint {
        Sparse,
        Dense,
    };

   public:
    explicit SimplexTree(size_t n_vertices);
    ~SimplexTree();

    // Non-copyable
    SimplexTree(const SimplexTree&) = delete;
    SimplexTree& operator=(const SimplexTree&) = delete;

    // Movable
    SimplexTree(SimplexTree&& other) noexcept;
    SimplexTree& operator=(SimplexTree&& other) noexcept;

    void Add(VertexRange auto&& simplex, Position pos) {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        Node* current = nullptr;
        size_t depth = 1;

        for (const VertexId& vid : simplex) {
            Node** next = current ? &current->next[vid] : &root_[vid];
            if (!*next) {
                *next = new Node{current, vid};
                LinkToList(*next, depth, vid);
            }
            current = *next;
            ++depth;
        }

        if (current != nullptr) {
            current->pos = pos;
        }
    }

    void Add(std::initializer_list<VertexId> simplex, Position pos) {
        Add(std::views::all(simplex), pos);
    }

    bool Has(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        return IsValidNode(TraverseDownwards(std::move(simplex)));
    }

    bool Has(std::initializer_list<VertexId> simplex) const {
        return Has(std::views::all(simplex));
    }

    Position GetPos(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        Node* node = TraverseDownwards(std::move(simplex));
        return IsValidNode(node) ? node->pos : kUnknownPos;
    }

    Position GetPos(std::initializer_list<VertexId> simplex) const {
        return GetPos(std::views::all(simplex));
    }

    Positions GetFacetsPos(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        Positions results;
        const size_t k = simplex.size();
        if (k <= 1) {
            return results;
        }

        Node* current = TraverseDownwards(simplex);
        if (!IsValidNode(current)) {
            return results;
        }

        for (size_t excluded = k; excluded > 0; --excluded) {
            current = current->previous;
            auto remaining = std::ranges::drop_view(simplex, excluded);
            Node* facet = remaining.empty()
                              ? current
                              : TraverseDownwards(remaining, current);
            if (IsValidNode(facet)) {
                results.emplace_back(facet->pos);
            }
        }

        return results;
    }

    Positions GetFacetsPos(std::initializer_list<VertexId> simplex) const {
        return GetFacetsPos(std::views::all(simplex));
    }

    Positions GetCofacetsPos(VertexRange auto&& simplex,
                             Hint hint = Hint::Sparse) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        Positions results;
        const size_t k = simplex.size();

        if (k == 0) {
            for (const auto& node : root_) {
                if (IsValidNode(node)) {
                    results.emplace_back(node->pos);
                }
            }
            return results;
        }

        if (k + 1 >= lists_heads_.size()) {
            return results;
        }

        Node* base = TraverseDownwards(simplex);
        if (!IsValidNode(base)) {
            return results;
        }

        for (const auto& [_, child] : base->next) {
            if (IsValidNode(child)) {
                results.emplace_back(child->pos);
            }
        }

        switch (hint) {
            case Hint::Sparse: {
                const auto& depth_map = lists_heads_[k + 1];
                auto it = depth_map.find(simplex.back());
                if (it == depth_map.end()) {
                    break;
                }
                FindSparseCofacets(base, it->second, results);
            } break;
            case Hint::Dense:
                FindDenseCofacets(simplex, results);
                break;
        }

        return results;
    }

    Positions GetCofacetsPos(std::initializer_list<VertexId> simplex,
                             Hint hint = Hint::Sparse) const {
        return GetCofacetsPos(std::views::all(simplex), hint);
    }

   private:
    static bool IsValidNode(Node* node);
    static bool IsSubsimplex(Node* node, Node* subnode, size_t skips);

    void DeleteSubtree(Node* node);
    void LinkToList(Node* node, size_t depth, VertexId id);

    Node* TraverseDownwards(VertexRange auto&& simplex,
                            Node* start = nullptr) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        Node* current = start;
        for (const VertexId& vid : simplex) {
            if (current == nullptr) {
                current = root_[vid];
            } else {
                auto it = current->next.find(vid);
                if (it == current->next.end()) {
                    return nullptr;
                }
                current = it->second;
            }
            if (current == nullptr) {
                return nullptr;
            }
        }
        return current;
    }

    void FindSparseCofacets(Node* base, Node* node, Positions& results) const {
        while (node != nullptr) {
            if (IsValidNode(node) && IsSubsimplex(node, base, 1)) {
                results.emplace_back(node->pos);
            }
            node = node->sibling;
        }
    }

    void FindDenseCofacets(const auto& simplex, Positions& results) const {
        const VertexId n = root_.size();
        auto it = simplex.begin();

        for (VertexId vid = 0; vid < n; ++vid) {
            if (*it == vid) {
                ++it;
                if (it == simplex.end()) {
                    break;
                }
                continue;
            }

            std::vector<VertexId> coface;
            coface.reserve(simplex.size() + 1);
            bool pushed_vid = false;
            for (const VertexId& i : simplex) {
                if (i >= vid && !pushed_vid) {
                    coface.emplace_back(vid);
                    pushed_vid = true;
                }
                coface.emplace_back(i);
            }
            if (!pushed_vid) {
                coface.emplace_back(vid);
            }
            assert(coface.size() == simplex.size() + 1);

            if (Position pos = GetPos(coface); pos != kUnknownPos) {
                results.emplace_back(pos);
            }
        }
    }

   private:
    RootNode root_;
    std::vector<NextMap> lists_heads_;
};

}  // namespace topa::common
