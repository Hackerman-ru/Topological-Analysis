#pragma once

#include "topa/common/type/position.hpp"

#include <vector>

namespace topa::models {

template <typename DerivedImpl>
class Matrix {
   public:
    using Row = std::vector<Position>;

    const Row& At(std::size_t col) const {
        return static_cast<const DerivedImpl*>(this)->At(col);
    }

    const Row& operator[](std::size_t col) const {
        return static_cast<const DerivedImpl*>(this)->operator[](col);
    }

    bool Contains(std::size_t col) const {
        return static_cast<const DerivedImpl*>(this)->Contains(col);
    }

    std::size_t Size() const {
        return static_cast<const DerivedImpl*>(this)->Size();
    }

    template <PositionRange R>
    void Insert(std::size_t col, R&& row) {
        return static_cast<DerivedImpl*>(this)->Insert(col,
                                                       std::forward<R>(row));
    }

    void Insert(std::size_t col, std::initializer_list<Position> row) {
        Insert(col, std::views::all(row));
    }

    Row& operator[](std::size_t col) {
        return static_cast<DerivedImpl*>(this)->operator[](col);
    }

    void Reserve(std::size_t n_cols) {
        return static_cast<DerivedImpl*>(this)->Reserve(n_cols);
    }

    void Erase(std::size_t col) {
        return static_cast<DerivedImpl*>(this)->Erase(col);
    }
};

}  // namespace topa::models
