#pragma once

#include "common/type/harmonic_pair.hpp"
#include "common/type/persistence_pair.hpp"

namespace topa::models {

template <typename DerivedImpl>
class Harmonic {
   public:
    using HarmonicPairs = std::vector<HarmonicPair>;

    template <typename Complex, PersistencePairRange R>
    static HarmonicPairs Compute(Complex&& complex, R&& pairs) {
        return DerivedImpl::Compute(std::forward<Complex>(complex),
                                    std::forward<R>(pairs));
    }
};

}  // namespace topa::models
