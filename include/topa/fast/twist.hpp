#pragma once

#include "topa/common/type/low.hpp"
#include "topa/common/boundary_matrix.hpp"
#include "topa/common/detail/process_matrix.hpp"
#include "topa/common/detail/low_to_pairs.hpp"

#include "topa/models/persistence_diagram.hpp"
#include "topa/models/filtered_complex.hpp"
#include "topa/models/matrix.hpp"
#include "topa/models/pos_heap.hpp"

namespace topa::fast {

template <typename Matrix, typename HeapImpl>
class Twist final : public models::PersistenceDiagram<Twist<Matrix, HeapImpl>> {
    using Arglows = std::vector<Position>;
    using Lows = std::vector<Low>;
    using PersistencePairs = typename models::PersistenceDiagram<
        Twist<Matrix, HeapImpl>>::PersistencePairs;

   public:
    template <typename ComplexImpl>
    static PersistencePairs Compute(
        const models::FilteredComplex<ComplexImpl>& complex) {
        Matrix boundary_matrix =
            common::BoundaryMatrix<Matrix, ComplexImpl>(complex);
        Lows lows = Reduce(boundary_matrix, complex);
        return detail::LowToPairs(std::move(lows));
    }

   private:
    template <typename ComplexImpl>
    static Lows Reduce(models::Matrix<Matrix>& matrix,
                       const models::FilteredComplex<ComplexImpl>& complex) {
        std::array positions = {complex.GetPosesBySize(3) | std::views::all,
                                complex.GetPosesBySize(2) | std::views::all};
        return detail::ProcessMatrix<Matrix, HeapImpl>(
            matrix, complex.Size(), positions | std::views::join);
    }
};

};  // namespace topa::fast
