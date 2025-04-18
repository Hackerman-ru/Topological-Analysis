#include "reducer.hpp"
#include "filtered_complex.hpp"
#include "boundary_matrix.hpp"
#include "detail/bit_tree_column.hpp"

#include <ranges>

namespace topa {

class ReduceSession {
    using Arglow = basic_types::DefaultContainer<size_t>;

   public:
    explicit ReduceSession(size_t simplex_number)
        : lows_(simplex_number, kNoneLow),
          arglow_(simplex_number, kNoneLow),
          fast_column_(simplex_number) {
    }

    void ReduceMatrix(Matrix& matrix, auto&& poses) {
        for (const Position& pos : poses) {
            if (!matrix.contains(pos)) {
                continue;
            }

            fast_column_.Add(matrix[pos]);
            Position pivot = fast_column_.GetMaxPos();

            while (!fast_column_.Empty() && arglow_[pivot] != kNoneLow) {
                fast_column_.Add(matrix[arglow_[pivot]]);
                pivot = fast_column_.GetMaxPos();
            }

            if (!fast_column_.Empty()) {
                arglow_[pivot] = pos;
                matrix.erase(pivot);
                lows_[pivot] = kNoneLow;
            }
            lows_[pos] = pivot;
            matrix[pos] = fast_column_.PopAll();
        }
    }

    void Clear() {
        std::ranges::fill(lows_, kNoneLow);
        std::ranges::fill(arglow_, kNoneLow);
    }

    Lows GetLows() const {
        return lows_;
    }

   private:
    Lows lows_;
    Arglow arglow_;
    detail::BitTreeColumn fast_column_;
};

class Reducer::TwistReducer : public Reducer::IReducer {
   public:
    Lows Reduce(const FilteredComplex& complex) const override {
        const size_t n = complex.GetSimplices().size();
        ReduceSession session(n);
        Matrix matrix = BoundaryMatrix(complex);
        session.ReduceMatrix(matrix, complex.GetPosesBySize(3));
        session.ReduceMatrix(matrix, complex.GetPosesBySize(2));
        return session.GetLows();
    }
};

class Reducer::DoubleTwistReducer : public Reducer::IReducer {
   public:
    Lows Reduce(const FilteredComplex& complex) const override {
        const size_t n = complex.GetSimplices().size();
        ReduceSession session(n);
        Matrix matrix = CoboundaryMatrix(complex);
        auto rv1 = complex.GetPosesBySize(1) |
                   std::views::transform([n](const Position& pos) -> Position {
                       return n - 1 - pos;
                   });
        session.ReduceMatrix(matrix, rv1);
        auto rv2 = complex.GetPosesBySize(2) |
                   std::views::transform([n](const Position& pos) -> Position {
                       return n - 1 - pos;
                   });
        session.ReduceMatrix(matrix, rv2);

        auto saved_pos = GetSavedPos(session.GetLows(), n);
        session.Clear();
        matrix = BoundaryMatrixFiltered(complex, saved_pos);

        session.ReduceMatrix(matrix, complex.GetPosesBySize(3));
        session.ReduceMatrix(matrix, complex.GetPosesBySize(2));
        return session.GetLows();
    }

   private:
    static basic_types::DefaultContainer<Position> GetSavedPos(const Lows& lows,
                                                               size_t n) {
        basic_types::DefaultContainer<Position> result;
        for (size_t i = 0; i < n; ++i) {
            if (lows[i] != kNoneLow) {
                result.emplace_back(n - 1 - lows[i]);
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

Lows Reducer::Reduce(const FilteredComplex& complex) const {
    return strategy_->Reduce(complex);
}

Reducer::Reducer(std::unique_ptr<IReducer> strategy)
    : strategy_(std::move(strategy)) {
}

}  // namespace topa
