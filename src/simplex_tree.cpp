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

void SimplexTree::Add(SortedInitializerList simplex, size_t pos) {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Simplex temp(std::move(simplex));
    Add(std::span{temp}, pos);
}

void SimplexTree::Add(SortedSimplexView simplex, size_t pos) {
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

bool SimplexTree::IsValid(Node* node) {
    return node != nullptr && node->pos != kNone;
}

Simplex SimplexTree::SimplexFrom(Node* node) {
    Simplex result;
    while (node != nullptr) {
        result.push_back(node->vertex_id);
        node = node->previous;
    }
    std::reverse(result.begin(), result.end());
    assert(std::is_sorted(result.begin(), result.end()));
    return result;
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

SimplexTree::CFaces SimplexTree::GetFacetsOf(
    SortedInitializerList simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Simplex temp(std::move(simplex));
    return GetFacetsOf(std::span{temp});
}

SimplexTree::CFaces SimplexTree::GetFacetsOf(SortedSimplexView simplex) const {
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

        Simplex facet_simplex;
        facet_simplex.reserve(k - 1);
        for (size_t i = 0; i < k; ++i) {
            if (i != excluded - 1) {
                facet_simplex.push_back(i);
            }
        }
        result.push_back(std::move(facet_simplex));
    }

    return result;
}

SimplexTree::CFaces SimplexTree::GetCofacetsOf(
    SortedInitializerList simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    Simplex temp(std::move(simplex));
    return GetCofacetsOf(std::span{temp});
}

SimplexTree::CFaces SimplexTree::GetCofacetsOf(
    SortedSimplexView simplex) const {
    assert(std::is_sorted(simplex.begin(), simplex.end()));
    CFaces result;
    const size_t k = simplex.size();

    if (k == 0) {
        for (const auto& node : root_) {
            if (!IsValid(node)) {
                continue;
            }
            result.push_back({node->vertex_id});
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
    for (const auto& [vid, child] : src_node->next) {
        if (!IsValid(child)) {
            continue;
        }
        coface.push_back(vid);
        result.push_back(coface);
        coface.pop_back();
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

        coface = SimplexFrom(current_node);
        if (std::includes(coface.begin(), coface.end(), simplex.begin(),
                          simplex.end())) {
            result.push_back(coface);
        }
    }

    return result;
}

}  // namespace topa
