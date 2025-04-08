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

   private:
    struct Node;
    using NextMap = basic_types::DefaultMap<VertexId, Node*>;
    using RootNode = basic_types::DefaultContainer<Node*>;

    static constexpr size_t kNone = std::numeric_limits<size_t>::max();

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

    bool Has(SortedInitializerList simplex) const;
    bool Has(SortedSimplexView simplex) const;

    void Add(SortedInitializerList simplex, size_t pos);
    void Add(SortedSimplexView simplex, size_t pos);

    CFaces GetFacetsOf(SortedInitializerList simplex) const;
    CFaces GetFacetsOf(SortedSimplexView simplex) const;

    CFaces GetCofacetsOf(SortedInitializerList simplex) const;
    CFaces GetCofacetsOf(SortedSimplexView simplex) const;

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
