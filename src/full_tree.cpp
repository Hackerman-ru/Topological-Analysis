#include "topa/fast/full_tree.hpp"

namespace topa::fast {

FullTree::FullTree(size_t n_vertices)
    : n_(n_vertices),
      vertex_pos_(n_, kUnknownPos),
      edge_pos_(n_ * (n_ - 1) / 2, kUnknownPos),
      triangle_pos_(n_ * (n_ - 1) * (n_ - 2) / 6, kUnknownPos) {
}

const Position& FullTree::VertexPos(VertexId u) const {
    assert(u < n_);
    return vertex_pos_[u];
}

const Position& FullTree::EdgePos(VertexId u, VertexId v) const {
    assert(u < v && v < n_);
    return edge_pos_[EdgeIndex(u, v)];
}

const Position& FullTree::TrianglePos(VertexId u, VertexId v,
                                      VertexId w) const {
    assert(u < v && v < w && w < n_);
    return triangle_pos_[TriangleIndex(u, v, w)];
}

Position& FullTree::VertexPos(VertexId u) {
    assert(u < n_);
    return vertex_pos_[static_cast<size_t>(u)];
}
Position& FullTree::EdgePos(VertexId u, VertexId v) {
    assert(u < v && v < n_);
    return edge_pos_[EdgeIndex(u, v)];
}
Position& FullTree::TrianglePos(VertexId u, VertexId v, VertexId w) {
    assert(u < v && v < w && w < n_);
    return triangle_pos_[TriangleIndex(u, v, w)];
}

size_t FullTree::EdgeIndex(VertexId u, VertexId v) const {
    return u * (2 * n_ - u - 1) / 2 + (v - u - 1);
}

size_t FullTree::TriangleIndex(VertexId u, VertexId v, VertexId w) const {
    return w * (w - 1) * (w - 2) / 6 + v * (v - 1) / 2 + u;
}

}  // namespace topa::fast
