#pragma once

#include "common/type/low.hpp"
#include "common/coboundary_matrix.hpp"
#include "common/filtered_boundary_matrix.hpp"
#include "common/detail/process_matrix.hpp"
#include "common/detail/low_to_pairs.hpp"

#include "models/persistence_diagram.hpp"
#include "models/filtered_complex.hpp"
#include "models/matrix.hpp"
#include "models/pos_heap.hpp"

namespace topa::fast {

template <typename Matrix, typename HeapImpl>
class DoubleTwist final
    : models::PersistenceDiagram<DoubleTwist<Matrix, HeapImpl>> {
    using Arglows = std::vector<Position>;
    using Lows = std::vector<Low>;
    using PersistencePairs = typename models::PersistenceDiagram<
        DoubleTwist<Matrix, HeapImpl>>::PersistencePairs;

   public:
    template <typename ComplexImpl>
    static PersistencePairs Compute(
        const models::FilteredComplex<ComplexImpl>& complex) {
        Matrix coboundary_matrix =
            common::CoboundaryMatrix<Matrix, ComplexImpl>(complex);
        Lows lows = Reduce(coboundary_matrix, complex);
        return detail::LowToPairs(std::move(lows));
    }

   private:
    template <typename ComplexImpl>
    static Lows Reduce(models::Matrix<Matrix>& matrix,
                       const models::FilteredComplex<ComplexImpl>& complex) {
        const auto n = complex.Size();
        // 1. Reducing coboundary matrix
        const auto inv = [n](Position pos) {
            return n - 1 - pos;
        };
        std::array copositions = {
            complex.GetPosesBySize(1) | std::views::transform(inv),
            complex.GetPosesBySize(2) | std::views::transform(inv)};
        Lows lows = detail::ProcessMatrix<Matrix, HeapImpl>(
            matrix, n, copositions | std::views::join);
        // 2. Computing saved poses
        std::vector<Position> saved_poses;
        for (const auto& low : lows) {
            if (low != kUnknownLow) {
                saved_poses.emplace_back(n - 1 - low);
            }
        }
        // 3. Reducing filtered boundary matrix
        Matrix boundary_matrix =
            common::BoundaryMatrixFiltered<Matrix, ComplexImpl>(
                complex, std::move(saved_poses));
        std::array positions = {complex.GetPosesBySize(3) | std::views::all,
                                complex.GetPosesBySize(2) | std::views::all};
        return detail::ProcessMatrix<Matrix, HeapImpl>(
            boundary_matrix, n, positions | std::views::join);
    }
};

};  // namespace topa::fast
