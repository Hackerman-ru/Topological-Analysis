#pragma once

#include "common/type/persistence_pair.hpp"

#include <vector>

namespace topa::models {

template <typename DerivedImpl>
class PersistenceDiagram {
   public:
    using PersistencePairs = std::vector<PersistencePair>;

    template <typename FilteredComplex>
    static PersistencePairs Compute(FilteredComplex&& complex) {
        return DerivedImpl::Compute(std::forward<FilteredComplex>(complex));
    }
};

}  // namespace topa::models
