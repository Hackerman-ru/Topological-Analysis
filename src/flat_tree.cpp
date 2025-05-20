#include "topa/fast/flat_tree.hpp"

namespace topa::fast {

FlatTree::FlatTree(size_t n_vertices)
    : n_(n_vertices),
      vertex_pos_(n_, kUnknownPos),
      edge_pos_(n_ * (n_ - 1) / 2, kUnknownPos),
      triangle_pos_(n_ * (n_ - 1) * (n_ - 2) / 6, kUnknownPos) {
}

const Position& FlatTree::VertexPos(VertexId u) const {
    assert(u < n_);
    return vertex_pos_[u];
}

const Position& FlatTree::EdgePos(VertexId u, VertexId v) const {
    assert(u < v && v < n_);
    return edge_pos_[EdgeIndex(u, v)];
}

const Position& FlatTree::TrianglePos(VertexId u, VertexId v,
                                      VertexId w) const {
    assert(u < v && v < w && w < n_);
    return triangle_pos_[TriangleIndex(u, v, w)];
}

Position& FlatTree::VertexPos(VertexId u) {
    assert(u < n_);
    return vertex_pos_[static_cast<size_t>(u)];
}
Position& FlatTree::EdgePos(VertexId u, VertexId v) {
    assert(u < v && v < n_);
    return edge_pos_[EdgeIndex(u, v)];
}
Position& FlatTree::TrianglePos(VertexId u, VertexId v, VertexId w) {
    assert(u < v && v < w && w < n_);
    return triangle_pos_[TriangleIndex(u, v, w)];
}

size_t FlatTree::EdgeIndex(VertexId u, VertexId v) const {
    return u * (2 * n_ - u - 1) / 2 + (v - u - 1);
}

size_t FlatTree::TriangleIndex(VertexId u, VertexId v, VertexId w) const {
    return w * (w - 1) * (w - 2) / 6 + v * (v - 1) / 2 + u;
}

}  // namespace topa::fast
