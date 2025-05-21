#pragma once

#include "topa/common/type/persistence_pair.hpp"
#include "topa/common/type/filtration_value.hpp"

#include "topa/models/filtered_complex.hpp"

#include <vector>
#include <set>

namespace topa::detail {

using FiltrationData =
    std::tuple<std::size_t, FiltrationValue, FiltrationValue>;
using FiltrationValues = std::vector<FiltrationData>;

template <typename ComplexImpl>
FiltrationValues PairsToFiltrationValues(
    const models::FilteredComplex<ComplexImpl>& complex,
    PersistencePairRange auto&& pairs) {
    FiltrationValues result;
    std::set<FiltrationData> unique_data;
    result.reserve(pairs.size());
    for (const PersistencePair& pair : pairs) {
        FiltrationValue birth = complex.GetFiltrationValue(pair.birth);
        FiltrationValue death = complex.GetFiltrationValue(pair.death);
        std::size_t dim = complex.GetSizeByPos(pair.birth) - 1;
        FiltrationData data(dim, birth, death);
        if (unique_data.insert(data).second) {
            result.emplace_back(std::move(data));
        }
    }
    return result;
}

}  // namespace topa::detail
