#ifndef TOPA_SIMPLEX_TREE_HPP_
#define TOPA_SIMPLEX_TREE_HPP_

#include "basic_types.hpp"
#include "position.hpp"
#include "simplex.hpp"

#include <algorithm>
#include <functional>
#include <limits>

namespace topa {

class SimplexTree {
   public:
    using SortedInitializerList = std::initializer_list<VertexId>;
    using SortedSimplexView = basic_types::DefaultView<const VertexId>;
    using CFaces = basic_types::DefaultContainer<Position>;

   private:
    struct Node;
    using NextMap = basic_types::DefaultMap<VertexId, Node*>;
    using RootNode = basic_types::DefaultContainer<Node*>;

    struct Node {
        Node* previous = nullptr;
        Node* sibling = nullptr;
        VertexId vertex_id;
        Position pos = kNonePos;
        NextMap next;
    };

   public:
    explicit SimplexTree(size_t vertices_number);
    ~SimplexTree();

    // Non-copyable
    SimplexTree(const SimplexTree&) = delete;
    SimplexTree& operator=(const SimplexTree&) = delete;

    // Movable
    SimplexTree(SimplexTree&& other) noexcept;
    SimplexTree& operator=(SimplexTree&& other) noexcept;

    void Add(SortedInitializerList simplex, Position pos);
    void Add(SortedSimplexView simplex, Position pos);

    bool Has(SortedInitializerList simplex) const;
    bool Has(SortedSimplexView simplex) const;

    CFaces GetFacets(SortedInitializerList simplex) const;
    CFaces GetFacets(SortedSimplexView simplex) const;

    CFaces GetCofacets(SortedInitializerList simplex) const;
    CFaces GetCofacets(SortedSimplexView simplex) const;

    Position GetPosition(SortedInitializerList simplex) const;
    Position GetPosition(SortedSimplexView simplex) const;

    size_t GetVerticesNumber() const;

   private:
    static bool IsValid(Node* node);
    static bool IsSubsimplex(Node* node, Node* subnode, size_t skips);

    void DeleteSubtree(Node* node);
    void LinkToList(Node* node, size_t depth, VertexId id);
    Node* TraverseDownwards(SortedSimplexView simplex,
                            Node* current_node = nullptr) const;

   private:
    RootNode root_;
    basic_types::DefaultContainer<NextMap> lists_heads_;
};

}  // namespace topa

#endif
