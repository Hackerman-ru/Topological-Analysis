#pragma once

#include "models/matrix.hpp"

#include <unordered_map>

namespace topa::fast {

class SparseMatrix final : public models::Matrix<SparseMatrix> {
    using Map = std::unordered_map<std::size_t, Row>;

   public:
    const Row& At(std::size_t col) const;
    const Row& operator[](std::size_t col) const;
    bool Contains(std::size_t col) const;
    std::size_t Size() const;

    void Insert(std::size_t col, PositionRange auto&& row) {
        map_.try_emplace(col, std::make_move_iterator(row.begin()),
                         std::make_move_iterator(row.end()));
    }

    void Insert(std::size_t col, std::initializer_list<Position> row) {
        Insert(col, std::views::all(row));
    }

    Row& operator[](std::size_t col);
    void Reserve(std::size_t n_cols);
    void Erase(std::size_t col);

   private:
    Map map_;
};

}  // namespace topa::fast
