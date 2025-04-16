#include "simplex_tree.hpp"

#include <cassert>
#include <cmath>
#include <unordered_set>

namespace topa {

SimplexTree::SimplexTree(size_t vertices_number)
    : root_(RootNode(vertices_number, nullptr)) {
}

SimplexTree::~SimplexTree() {
    for (auto& node : root_) {
        DeleteSubtree(node);
    }
}

SimplexTree::SimplexTree(SimplexTree&& other) noexcept
    : root_(std::move(other.root_)),
      lists_heads_(std::move(other.lists_heads_)) {
    other.root_.clear();
    other.lists_heads_.clear();
}

SimplexTree& SimplexTree::operator=(SimplexTree&& other) noexcept {
    if (this != &other) {
        for (auto& node : root_) {
            DeleteSubtree(node);
        }

        root_ = std::move(other.root_);
        lists_heads_ = std::move(other.lists_heads_);

        other.root_.clear();
        other.lists_heads_.clear();
    }
    return *this;
}

void SimplexTree::Add(SortedInitializerList simplex, Position pos) {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Simplex temp(std::move(simplex));
    Add(std::span{temp}, pos);
}

void SimplexTree::Add(SortedSimplexView simplex, Position pos) {
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

bool SimplexTree::Has(SortedInitializerList simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Simplex temp(std::move(simplex));
    return Has(std::span{temp});
}

bool SimplexTree::Has(SortedSimplexView simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Node* node = TraverseDownwards(std::move(simplex));
    return IsValid(node);
}

bool SimplexTree::IsValid(Node* node) {
    return node != nullptr && node->pos != kNonePos;
}

bool SimplexTree::IsSubsimplex(Node* node, Node* subnode, size_t skips) {
    while (node != nullptr) {
        if (subnode == nullptr) {
            return true;
        }
        if (node->vertex_id == subnode->vertex_id) {
            node = node->previous;
            subnode = subnode->previous;
            continue;
        }
        if (skips > 0) {
            --skips;
            node = node->previous;
            continue;
        }
        return false;
    }
    return subnode == nullptr;
}

void SimplexTree::DeleteSubtree(Node* node) {
    if (node == nullptr) {
        return;
    }
    for (auto& [_, child] : node->next) {
        DeleteSubtree(child);
    }
    delete node;
}

void SimplexTree::LinkToList(Node* node, size_t depth, VertexId id) {
    if (lists_heads_.size() <= depth) {
        lists_heads_.resize(depth + 1);
    }
    // (iterator to (key, value); bool)
    auto pair = lists_heads_[depth].try_emplace(id, nullptr);
    node->sibling = (*pair.first).second;
    lists_heads_[depth][id] = node;
}

SimplexTree::Node* SimplexTree::TraverseDownwards(SortedSimplexView simplex,
                                                  Node* current_node) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
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

SimplexTree::CFaces SimplexTree::GetFacets(
    SortedInitializerList simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Simplex temp(std::move(simplex));
    return GetFacets(std::span{temp});
}

SimplexTree::CFaces SimplexTree::GetFacets(SortedSimplexView simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    CFaces result;
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
        SortedSimplexView rhs_part = simplex.subspan(excluded);

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

SimplexTree::CFaces SimplexTree::GetCofacets(
    SortedInitializerList simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Simplex temp(std::move(simplex));
    return GetCofacets(std::span{temp});
}

SimplexTree::CFaces SimplexTree::GetCofacets(SortedSimplexView simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    CFaces result;
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

    Simplex coface(simplex.begin(), simplex.end());
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

Position SimplexTree::GetPosition(SortedInitializerList simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Simplex temp(std::move(simplex));
    return GetPosition(std::span{temp});
}

Position SimplexTree::GetPosition(SortedSimplexView simplex) const {
    Node* node = TraverseDownwards(simplex);
    return IsValid(node) ? node->pos : kNonePos;
}

size_t SimplexTree::GetVerticesNumber() const {
    return root_.size();
}

}  // namespace topa
