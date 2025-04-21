#include "fast/full_tree.hpp"

namespace topa::common::complex {

FullTree::FullTree(size_t vertices_number)
    : root_(RootNode(vertices_number, nullptr)) {
}

FullTree::~FullTree() {
    for (auto& node : root_) {
        DeleteSubtree(node);
    }
}

FullTree::FullTree(FullTree&& other) noexcept
    : root_(std::move(other.root_)) {
    other.root_.clear();
}

FullTree& FullTree::operator=(FullTree&& other) noexcept {
    if (this != &other) {
        for (auto& node : root_) {
            DeleteSubtree(node);
        }

        root_ = std::move(other.root_);
        other.root_.clear();
    }
    return *this;
}

bool FullTree::IsValid(Node* node) {
    return node != nullptr && node->pos != kUnknownPos;
}

bool FullTree::IsSubsimplex(Node* node, Node* subnode, size_t skips) {
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

void FullTree::DeleteSubtree(Node* node) {
    if (node == nullptr) {
        return;
    }
    for (auto& [_, child] : node->next) {
        DeleteSubtree(child);
    }
    delete node;
}

}  // namespace topa::common::complex
