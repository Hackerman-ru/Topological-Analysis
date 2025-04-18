#ifndef TOPA_LOW_HPP_
#define TOPA_LOW_HPP_

#include "basic_types.hpp"

#include <cstddef>
#include <limits>

namespace topa {

using Low = size_t;
using Lows = basic_types::DefaultContainer<Low>;
constexpr Low kNoneLow = std::numeric_limits<size_t>::max();

}  // namespace topa

#endif
