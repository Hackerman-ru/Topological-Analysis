#include "topa/fast/sparse_matrix.hpp"

#include <cassert>

namespace topa::fast {

const SparseMatrix::Row& SparseMatrix::At(std::size_t col) const {
    return map_.at(col);
}

const SparseMatrix::Row& SparseMatrix::operator[](std::size_t col) const {
    assert(Contains(col));
    return map_.at(col);
}

bool SparseMatrix::Contains(std::size_t col) const {
    return map_.contains(col);
}

std::size_t SparseMatrix::Size() const {
    return map_.size();
}

SparseMatrix::Row& SparseMatrix::operator[](std::size_t col) {
    return map_[col];
}

void SparseMatrix::Reserve(std::size_t n_cols) {
    map_.reserve(n_cols);
}

void SparseMatrix::Erase(std::size_t col) {
    map_.erase(col);
}

}  // namespace topa::fast
