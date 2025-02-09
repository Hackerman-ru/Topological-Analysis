#ifndef TOPA_SIMPLEX_TREE_HPP
#define TOPA_SIMPLEX_TREE_HPP

#include <algorithm>
#include <boost/container/flat_map.hpp>
#include <cstddef>
#include <iterator>
#include <vector>

namespace topa {
    class SimplexTree {
        using Id = size_t;
        using TemporaryContainer = std::vector<Id>;

        struct Node;
        using Map = boost::container::flat_map<Id, Node*>;
        using RootNode = std::vector<Node*>;

        struct Node {
            Node* previous = nullptr;
            Node* sibling = nullptr;
            Id vertex_id;
            Map next;
        };

        using CofacesSubtrees = std::vector<Node*>;
        using Faces = std::vector<Node*>;

    public:
        SimplexTree(size_t vertices_number) : root(RootNode(vertices_number, nullptr)) {}

        template<typename Container>
        bool has_simplex(const Container& simplex_vertices_ids) const {
            TemporaryContainer ids = sort_ids(simplex_vertices_ids);
            return can_traverse_downwards(std::begin(ids), std::end(ids));
        }

        template<typename Container>
        void add_simplex(const Container& simplex_vertices_ids) {
            TemporaryContainer ids = sort_ids(simplex_vertices_ids);
            append_downwards(std::begin(ids), std::end(ids));
        }

        template<typename Container>
        void add_simplex_and_all_subfaces(const Container& simplex_vertices_ids) {
            TemporaryContainer ids = sort_ids(simplex_vertices_ids);
            recursively_append_downwards(std::begin(ids), std::end(ids));
        }

    private:
        template<typename Container>
        static TemporaryContainer sort_ids(const Container& simplex_vertices_ids) {
            TemporaryContainer ids(std::begin(simplex_vertices_ids), std::end(simplex_vertices_ids));
            std::sort(ids.begin(), ids.end());
            return ids;
        }

        template<typename InputIterator>
        void recursively_append_downwards(InputIterator begin, InputIterator end,
                                          Node* current_node = nullptr, size_t depth = 0) {
            for (InputIterator it = begin; it != end;) {
                Id id = *it;
                Node** next_node = nullptr;
                if (current_node == nullptr) {
                    next_node = &root[id];
                } else {
                    next_node = &current_node->next[id];
                }
                if (*next_node == nullptr) {
                    *next_node = new Node;
                    (*next_node)->previous = current_node;
                    (*next_node)->vertex_id = id;
                    link_node_to_list(*next_node, depth, id);
                }
                recursively_append_downwards(++it, end, *next_node, depth + 1);
            }
        }

        void link_node_to_list(Node* node, size_t depth, Id id) {
            if (lists_heads.size() < depth) {
                lists_heads.resize(depth);
            }
            node->sibling = lists_heads[depth][id];
            lists_heads[depth][id] = node;
        }

        template<typename Container>
        CofacesSubtrees find_cofaces(const Container& simplex_vertices_ids) const {
            TemporaryContainer ids = sort_ids(simplex_vertices_ids);

            CofacesSubtrees subtrees;
            for (size_t i = ids.size(); i < lists_heads.size(); ++i) {
                const auto& list = lists_heads[i];
                auto pair_it = list.find(ids.back());
                if (pair_it == list.end()) {
                    continue;
                }
                Node* node = pair_it->second;
                if (can_traverse_upwards_with_skips(std::rbegin(ids), std::rend(ids), node)) {
                    subtrees.emplace_back(node);
                }
            }
            return subtrees;
        }

        template<typename Container>
        Faces find_faces(const Container& simplex_vertices_ids) const {
            TemporaryContainer ids = sort_ids(simplex_vertices_ids);

            Faces faces;
            Node* node = traverse_downwards(std::begin(ids), std::end(ids));
            auto begin = std::end(ids);
            auto end = std::end(ids);
            while (node != nullptr && begin != std::begin(ids)) {
                node = node->previous;
                Node* face = traverse_downwards(begin--, end, node);
                if (face != nullptr) {
                    faces.emplace_back(face);
                }
            }
            return faces;
        }

        template<typename InputIterator>
        Node* traverse_downwards(InputIterator begin, InputIterator end, Node* current_node = nullptr) const {
            for (auto it = begin; it != end; ++it) {
                Id id = *it;
                Node* next_node = nullptr;
                if (current_node == nullptr) {
                    next_node = root[id];
                } else {
                    auto next_pair_it = current_node->next.find(id);
                    if (next_pair_it == current_node->next.end()) {
                        return nullptr;
                    }
                    next_node = next_pair_it->second;
                }
                if (next_node == nullptr) {
                    return nullptr;
                }
                current_node = next_node;
            }
            return current_node;
        }

        template<typename InputIterator>
        Node* append_downwards(InputIterator begin, InputIterator end, Node* current_node = nullptr,
                               size_t depth = 1) {
            for (auto it = begin; it != end; ++it, ++depth) {
                Id id = *it;
                Node** next_node = nullptr;
                if (current_node == nullptr) {
                    next_node = &root[id];
                } else {
                    next_node = &current_node->next[id];
                }
                if (*next_node == nullptr) {
                    *next_node = new Node;
                    (*next_node)->previous = current_node;
                    (*next_node)->vertex_id = id;
                    link_node_to_list(*next_node, depth, id);
                }
                current_node = *next_node;
            }
            return current_node;
        }

        template<typename InputIterator>
        bool can_traverse_upwards_with_skips(InputIterator begin, InputIterator end,
                                             Node* current_node) const {
            for (auto it = begin; it != end; ++it) {
                Id id = *it;
                while (current_node != nullptr && current_node->vertex_id != id) {
                    current_node = current_node->previous;
                }
                if (current_node == nullptr) {
                    return false;
                }
                current_node = current_node->previous;
            }
            return true;
        }

        template<typename InputIterator>
        bool can_traverse_downwards(InputIterator begin, InputIterator end,
                                    Node* current_node = nullptr) const {
            return traverse_downwards(begin, end, current_node) != nullptr;
        }

        RootNode root;
        std::vector<Map> lists_heads;
    };
}   // namespace topa

#endif
