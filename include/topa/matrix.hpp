#ifndef TOPA_MATRIX_HPP_
#define TOPA_MATRIX_HPP_

#include "basic_types.hpp"
#include "position.hpp"

#include <string>

namespace topa {

using Matrix =
    basic_types::DefaultUnorderedMap<size_t,
                                     basic_types::DefaultContainer<Position>>;

void PrintMatrix(const Matrix& matrix, size_t n, const std::string& title);

}  // namespace topa

#endif
