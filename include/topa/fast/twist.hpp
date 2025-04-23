#pragma once

#include "common/type/low.hpp"
#include "common/boundary_matrix.hpp"
#include "common/process_matrix.hpp"
#include "common/low_to_pairs.hpp"

#include "models/persistence_diagram.hpp"
#include "models/filtered_complex.hpp"
#include "models/matrix.hpp"
#include "models/pos_heap.hpp"

namespace topa::fast {

template <typename MatrixImpl, typename HeapImpl>
class Twist final
    : public models::PersistenceDiagram<Twist<MatrixImpl, HeapImpl>> {
    using Arglows = std::vector<Position>;
    using Lows = std::vector<Low>;
    using PersistencePairs = typename models::PersistenceDiagram<
        Twist<MatrixImpl, HeapImpl>>::PersistencePairs;

   public:
    template <typename ComplexImpl>
    static PersistencePairs Compute(
        const models::FilteredComplex<ComplexImpl>& complex) {
        MatrixImpl boundary_matrix =
            common::BoundaryMatrix<MatrixImpl, ComplexImpl>(complex);
        Lows lows = Reduce(boundary_matrix, complex);
        return common::LowToPairs(std::move(lows));
    }

   private:
    template <typename ComplexImpl>
    static Lows Reduce(models::Matrix<MatrixImpl>& matrix,
                       const models::FilteredComplex<ComplexImpl>& complex) {
        std::array positions = {complex.GetPosesBySize(3) | std::views::all,
                                complex.GetPosesBySize(2) | std::views::all};
        return common::ProcessMatrix<MatrixImpl, HeapImpl>(
            matrix, complex.Size(), positions | std::views::join);
    }
};

};  // namespace topa::fast
