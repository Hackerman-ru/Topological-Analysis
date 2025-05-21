#include "topa/common/simplex_tree.hpp"

namespace topa::common {

SimplexTree::SimplexTree(size_t vertices_number)
    : root_(vertices_number, nullptr) {
}

SimplexTree::~SimplexTree() {
    for (auto& node : root_) {
        DeleteSubtree(node);
    }
}

SimplexTree::SimplexTree(SimplexTree&& other) noexcept
    : root_(std::move(other.root_)) {
    other.root_.clear();
}

SimplexTree& SimplexTree::operator=(SimplexTree&& other) noexcept {
    if (this != &other) {
        for (auto& node : root_) {
            DeleteSubtree(node);
        }

        root_ = std::move(other.root_);
        other.root_.clear();
    }
    return *this;
}

bool SimplexTree::IsValidNode(Node* node) {
    return node != nullptr && node->pos != kUnknownPos;
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

}  // namespace topa::common
