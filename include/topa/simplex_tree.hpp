#ifndef TOPA_SIMPLEX_TREE_HPP_
#define TOPA_SIMPLEX_TREE_HPP_

#include "basic_types.hpp"
#include "filtration.hpp"
#include "pointcloud.hpp"
#include "simplex.hpp"

#include <algorithm>
#include <functional>
#include <limits>

namespace topa {

class SimplexTree {
   public:
    using SortedInitializerList = std::initializer_list<VertexId>;
    using SortedSimplexView = basic_types::DefaultView<const VertexId>;
    using CFaces = basic_types::DefaultContainer<Simplex>;

    static constexpr size_t kNone = std::numeric_limits<size_t>::max();

   private:
    struct Node;
    using NextMap = basic_types::DefaultMap<VertexId, Node*>;
    using RootNode = basic_types::DefaultContainer<Node*>;

    struct Node {
        Node* previous = nullptr;
        Node* sibling = nullptr;
        VertexId vertex_id;
        // Position in the sorted list of filtered simplices
        size_t pos = kNone;
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

    void Add(SortedInitializerList simplex, size_t pos);
    void Add(SortedSimplexView simplex, size_t pos);

    bool Has(SortedInitializerList simplex) const;
    bool Has(SortedSimplexView simplex) const;

    CFaces GetFacets(SortedInitializerList simplex) const;
    CFaces GetFacets(SortedSimplexView simplex) const;

    CFaces GetCofacets(SortedInitializerList simplex) const;
    CFaces GetCofacets(SortedSimplexView simplex) const;

    size_t GetPosition(SortedSimplexView simplex) const;

   private:
    static bool IsValid(Node* node);
    static Simplex SimplexFrom(Node* node);

    void DeleteSubtree(Node* node);
    void LinkToList(Node* node, size_t depth, VertexId id);
    Node* TraverseDownwards(SortedSimplexView simplex,
                            Node* current_node = nullptr) const;

   private:
    RootNode root_;
    std::vector<NextMap> lists_heads_;
};

}  // namespace topa

#endif
