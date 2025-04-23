#pragma once

#include "common/type/low.hpp"

#include "models/filtered_complex.hpp"
#include "models/matrix.hpp"
#include "models/pos_heap.hpp"

namespace topa::common {

template <typename MatrixImpl, typename HeapImpl>
void ProcessPosition(Position pos, models::Matrix<MatrixImpl>& matrix,
                     models::PosHeap<HeapImpl>& fast_column,
                     std::vector<Position>& arglows) {
    if (!matrix.Contains(pos)) {
        return;
    }

    fast_column.Add(matrix[pos]);
    Position pivot = fast_column.GetMaxPos();

    while (!fast_column.IsEmpty() && arglows[pivot] != kUnknownPos) {
        fast_column.Add(matrix[arglows[pivot]]);
        pivot = fast_column.GetMaxPos();
    }

    if (!fast_column.IsEmpty()) {
        arglows[pivot] = pos;
        matrix.Erase(pivot);
    }

    matrix[pos] = fast_column.PopAll();
}

// std::views::concat will appear only in C++26
template <typename MatrixImpl, typename HeapImpl,
          std::ranges::input_range PosRange>
std::vector<Low> ProcessMatrix(models::Matrix<MatrixImpl>& matrix,
                               std::size_t n, PosRange&& positions) {
    std::vector<Position> arglows(n, kUnknownPos);
    HeapImpl fast_column(n);

    for (Position pos : positions) {
        ProcessPosition(pos, matrix, fast_column, arglows);
    }

    std::vector<Low> lows(n, kUnknownLow);
    for (size_t i = 0; i < n; ++i) {
        if (!matrix.Contains(i)) {
            continue;
        }
        const auto& row = matrix[i];
        if (!row.empty()) {
            lows[i] = row.back();
        }
    }
    return lows;
}

}  // namespace topa::common
