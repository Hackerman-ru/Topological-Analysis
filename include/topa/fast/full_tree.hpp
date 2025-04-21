#pragma once

#include "common/complex/simplex_tree/simplex_tree.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <map>
#include <cassert>

namespace topa::common::complex {

class FullTree : public SimplexTree<FullTree> {
   private:
    struct Node;
    using NextMap = std::map<VertexId, Node*>;
    using RootNode = std::vector<Node*>;

    struct Node {
        Node* previous = nullptr;
        Node* sibling = nullptr;
        VertexId vertex_id;
        Position pos = kUnknownPos;
        NextMap next;
    };

   public:
    explicit FullTree(size_t vertices_number);
    ~FullTree();

    // Non-copyable
    FullTree(const FullTree&) = delete;
    FullTree& operator=(const FullTree&) = delete;

    // Movable
    FullTree(FullTree&& other) noexcept;
    FullTree& operator=(FullTree&& other) noexcept;

    void Add(VertexRange auto&& simplex, Position pos) {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        Node* current_node = nullptr;
        size_t depth = 1;

        for (const VertexId& vid : simplex) {
            Node** next_node = nullptr;
            if (current_node == nullptr) {
                next_node = &root_[vid];
            } else {
                next_node = &current_node->next[vid];
            }
            if (*next_node == nullptr) {
                *next_node = new Node;
                (*next_node)->previous = current_node;
                (*next_node)->vertex_id = vid;
                LinkToList(*next_node, depth, vid);
            }
            current_node = *next_node;
            ++depth;
        }

        if (current_node != nullptr) {
            current_node->pos = pos;
        }
    }

    void Add(std::initializer_list<VertexId> simplex, Position pos) {
        Add(std::views::all(simplex), pos);
    }

    bool Has(VertexRange auto&& simplex) const {
        assert(std::is_sorted(simplex.begin(), simplex.end()));
        Node* node = TraverseDownwards(std::move(simplex));
        return IsValid(node);
    }

    bool Has(std::initializer_list<VertexId> simplex) const {
        return Has(std::views::all(simplex));
    }

    Positions GetFacetsPos(VertexRange auto&& simplex) const {
        Positions result;
        const size_t k = simplex.size();
        if (k <= 1) {
            return result;
        }

        Node* node = TraverseDownwards(simplex);
        if (!IsValid(node)) {
            return result;
        }

        for (size_t excluded = k; excluded > 0; --excluded) {
            node = node->previous;
            auto rhs_part = std::ranges::drop_view(simplex, excluded);

            Node* facet_node = node;
            if (!rhs_part.empty()) {
                facet_node = TraverseDownwards(rhs_part, node);
            }
            if (!IsValid(facet_node)) {
                continue;
            }
            result.emplace_back(facet_node->pos);
        }

        return result;
    }

    Positions GetFacetsPos(std::initializer_list<VertexId> simplex) const {
        return GetFacetsPos(std::views::all(simplex));
    }

    Positions GetCofacetsPos(VertexRange auto&& simplex) const {
        Positions result;
        const size_t k = simplex.size();

        if (k == 0) {
            for (const auto& node : root_) {
                if (!IsValid(node)) {
                    continue;
                }
                result.emplace_back(node->pos);
            }
            return result;
        }

        if (k + 1 >= lists_heads_.size()) {
            return result;
        }

        Node* src_node = TraverseDownwards(simplex);
        if (!IsValid(src_node)) {
            return result;
        }

        for (const auto& [_, child] : src_node->next) {
            if (!IsValid(child)) {
                continue;
            }
            result.emplace_back(child->pos);
        }

        const auto& depth_map = lists_heads_[k + 1];
        auto it = depth_map.find(simplex.back());
        if (it == depth_map.end()) {
            return result;
        }

        for (Node* current_node = it->second; current_node != nullptr;
             current_node = current_node->sibling) {
            if (!IsValid(current_node)) {
                continue;
            }
            if (IsSubsimplex(current_node, src_node, 1)) {
                result.emplace_back(current_node->pos);
            }
        }

        return result;
    }

    Positions GetCofacetsPos(std::initializer_list<VertexId> simplex) const {
        return GetCofacetsPos(std::views::all(simplex));
    }

    Position GetPos(VertexRange auto&& simplex) const {
        Node* node = TraverseDownwards(std::move(simplex));
        return IsValid(node) ? node->pos : kUnknownPos;
    }

    Position GetPos(std::initializer_list<VertexId> simplex) const {
        return GetPos(std::views::all(simplex));
    }

   private:
    static bool IsValid(Node* node);
    static bool IsSubsimplex(Node* node, Node* subnode, size_t skips);

    void DeleteSubtree(Node* node);

    Node* TraverseDownwards(VertexRange auto&& simplex,
                            Node* current_node = nullptr) const {
        for (const VertexId& vid : simplex) {
            if (current_node == nullptr) {
                current_node = root_[vid];
            } else {
                auto it = current_node->next.find(vid);
                if (it == current_node->next.end()) {
                    return nullptr;
                }
                current_node = it->second;
            }
            if (current_node == nullptr) {
                return nullptr;
            }
        }
        return current_node;
    }

   private:
    RootNode root_;
};

}  // namespace topa::common::complex
