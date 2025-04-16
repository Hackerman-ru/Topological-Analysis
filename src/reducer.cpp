#include "reducer.hpp"
#include "complex.hpp"
#include "boundary_matrix.hpp"
#include "detail/bit_tree_column.hpp"

#include <unordered_set>
#include <bits/chrono.h>
#include <iostream>

namespace topa {

static void ReduceMatrix(const WSimplices& simplices, Matrix<size_t>& matrix,
                         detail::BitTreeColumn& fast_column,
                         basic_types::DefaultView<size_t> sizes,
                         std::string name) {
    const size_t n = matrix.size();
    // size_t nonzero_entries = 0;
    // for (Position pos = 0; pos < n; ++pos) {
    //     nonzero_entries += matrix[pos].size();
    // }
    // std::cout << "Nonzero entries at start: " << nonzero_entries << '\n';
    auto start = std::chrono::system_clock::now();
    std::cout << "Start reducing " << name << "...\n";
    basic_types::DefaultContainer<size_t> arglow(n);

    for (const auto& size : sizes) {
        for (Position pos = 0; pos < n; ++pos) {
            if (simplices[pos].simplex.size() != size) {
                continue;
            }
            if (matrix[pos].empty()) {
                continue;
            }

            fast_column.Add(matrix[pos]);
            Position pivot = fast_column.GetMaxPos();

            while (!fast_column.Empty() && arglow[pivot] != 0) {
                fast_column.Add(matrix[arglow[pivot]]);
                pivot = fast_column.GetMaxPos();
            }

            if (!fast_column.Empty()) {
                arglow[pivot] = pos;
                matrix[pivot].clear();
            }
            matrix[pos] = fast_column.PopAll();
        }
    }
    auto end = std::chrono::system_clock::now();
    // nonzero_entries = 0;
    // for (Position pos = 0; pos < n; ++pos) {
    //     nonzero_entries += matrix[pos].size();
    // }
    // std::cout << "Nonzero entries at finish: " << nonzero_entries << '\n';
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Reduce elapsed time: " << elapsed_seconds.count() << "s"
              << '\n';
}

class Reducer::TwistReducer : public Reducer::IReducer {
   public:
    Matrix<size_t> Reduce(const Complex& complex) const override {
        auto start = std::chrono::system_clock::now();
        const auto& simplices = complex.GetSimplices();
        Matrix<size_t> matrix = BoundaryMatrix(complex);
        detail::BitTreeColumn fast_column(matrix.size());
        basic_types::DefaultContainer<size_t> sizes = {3u, 2u};
        ReduceMatrix(simplices, matrix, fast_column, sizes, "Boundary Matrix");
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << "Twist-Reduce total elapsed time: "
                  << elapsed_seconds.count() << "s" << '\n';
        return matrix;
    }
};

class Reducer::DoubleTwistReducer : public Reducer::IReducer {
   public:
    Matrix<size_t> Reduce(const Complex& complex) const override {
        auto start = std::chrono::system_clock::now();
        const auto& simplices = complex.GetSimplices();
        Matrix<size_t> matrix = CoboundaryMatrix(complex);
        detail::BitTreeColumn fast_column(matrix.size());
        basic_types::DefaultContainer<size_t> sizes = {1u, 2u};
        ReduceMatrix(simplices, matrix, fast_column, sizes,
                     "Coboundary Matrix");

        auto saved_pos = GetSavedPos(std::move(matrix));
        matrix = BoundaryMatrixFiltered(complex, saved_pos);
        sizes = {3u, 2u};
        ReduceMatrix(simplices, matrix, fast_column, sizes, "Boundary Matrix");
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << "DoubleTwist-Reduce total elapsed time: "
                  << elapsed_seconds.count() << "s" << '\n';
        return matrix;
    }

   private:
    static basic_types::DefaultContainer<Position> GetSavedPos(
        const Matrix<size_t>& matrix) {
        basic_types::DefaultContainer<Position> result;
        detail::BitTreeColumn fast_column(matrix.size());

        for (Position pos = 0; pos < matrix.size(); ++pos) {
            if (!matrix[pos].empty()) {
                fast_column.Add(matrix[pos]);
                result.emplace_back(matrix.size() - 1 -
                                    fast_column.GetMaxPos());
                fast_column.Clear();
            }
        }
        return result;
    }
};

Reducer Reducer::Twist() {
    return Reducer{std::make_unique<TwistReducer>()};
}

Reducer Reducer::DoubleTwist() {
    return Reducer{std::make_unique<DoubleTwistReducer>()};
}

Matrix<size_t> Reducer::Reduce(const Complex& complex) const {
    return strategy_->Reduce(complex);
}

Reducer::Reducer(std::unique_ptr<IReducer> strategy)
    : strategy_(std::move(strategy)) {
}

}  // namespace topa
