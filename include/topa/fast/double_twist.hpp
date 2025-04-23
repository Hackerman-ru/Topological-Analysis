#pragma once

#include "common/type/low.hpp"
#include "common/coboundary_matrix.hpp"
#include "common/filtered_boundary_matrix.hpp"
#include "common/process_matrix.hpp"
#include "common/low_to_pairs.hpp"

#include "models/persistence_diagram.hpp"
#include "models/filtered_complex.hpp"
#include "models/matrix.hpp"
#include "models/pos_heap.hpp"

namespace topa::fast {

template <typename MatrixImpl, typename HeapImpl>
class DoubleTwist final
    : models::PersistenceDiagram<DoubleTwist<MatrixImpl, HeapImpl>> {
    using Arglows = std::vector<Position>;
    using Lows = std::vector<Low>;
    using PersistencePairs = typename models::PersistenceDiagram<
        DoubleTwist<MatrixImpl, HeapImpl>>::PersistencePairs;

   public:
    template <typename ComplexImpl>
    static PersistencePairs Compute(
        const models::FilteredComplex<ComplexImpl>& complex) {
        MatrixImpl coboundary_matrix =
            common::CoboundaryMatrix<MatrixImpl, ComplexImpl>(complex);
        Lows lows = Reduce(coboundary_matrix, complex);
        return common::LowToPairs(std::move(lows));
    }

   private:
    template <typename ComplexImpl>
    static Lows Reduce(models::Matrix<MatrixImpl>& matrix,
                       const models::FilteredComplex<ComplexImpl>& complex) {
        const auto n = complex.Size();
        // 1. Reducing coboundary matrix
        const auto inv = [n](Position pos) {
            return n - 1 - pos;
        };
        std::array copositions = {
            complex.GetPosesBySize(1) | std::views::transform(inv),
            complex.GetPosesBySize(2) | std::views::transform(inv)};
        Lows lows = common::ProcessMatrix<MatrixImpl, HeapImpl>(
            matrix, n, copositions | std::views::join);
        // 2. Computing saved poses
        std::vector<Position> saved_poses;
        for (const auto& low : lows) {
            if (low != kUnknownLow) {
                saved_poses.emplace_back(n - 1 - low);
            }
        }
        // 3. Reducing filtered boundary matrix
        MatrixImpl boundary_matrix =
            common::BoundaryMatrixFiltered<MatrixImpl, ComplexImpl>(
                complex, std::move(saved_poses));
        std::array positions = {complex.GetPosesBySize(3) | std::views::all,
                                complex.GetPosesBySize(2) | std::views::all};
        return common::ProcessMatrix<MatrixImpl, HeapImpl>(
            boundary_matrix, n, positions | std::views::join);
    }
};

};  // namespace topa::fast
