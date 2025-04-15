#ifndef TOPA_MATRIX_HPP_
#define TOPA_MATRIX_HPP_

#include "basic_types.hpp"

namespace topa {

template <typename T>
using Matrix = basic_types::DefaultContainer<basic_types::DefaultContainer<T>>;

}  // namespace topa

#endif
