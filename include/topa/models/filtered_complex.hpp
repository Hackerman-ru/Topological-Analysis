#pragma once

#include "topa/common/type/position.hpp"
#include "topa/common/type/filtration_value.hpp"

#include <vector>

namespace topa::models {

template <typename DerivedImpl>
class FilteredComplex {
   public:
    using Positions = std::vector<Position>;

    std::size_t Size() const {
        return static_cast<const DerivedImpl*>(this)->Size();
    }
    FiltrationValue GetFiltrationValue(Position pos) const {
        return static_cast<const DerivedImpl*>(this)->GetFiltrationValue(pos);
    }
    std::size_t GetSizeByPos(Position pos) const {
        return static_cast<const DerivedImpl*>(this)->GetSizeByPos(pos);
    }
    const Positions& GetPosesBySize(std::size_t size) const {
        return static_cast<const DerivedImpl*>(this)->GetPosesBySize(size);
    }
    Positions GetFacetsPosition(Position pos) const {
        return static_cast<const DerivedImpl*>(this)->GetFacetsPosition(pos);
    }
    Positions GetCofacetsPosition(Position pos) const {
        return static_cast<const DerivedImpl*>(this)->GetCofacetsPosition(pos);
    }
};

}  // namespace topa::models
