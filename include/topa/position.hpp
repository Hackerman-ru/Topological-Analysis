#ifndef TOPA_POSITION_HPP_
#define TOPA_POSITION_HPP_

#include "basic_types.hpp"

#include <cstddef>
#include <limits>

namespace topa {

// Position in the sorted list of filtered simplices
using Position = size_t;
using Positions = basic_types::DefaultContainer<Position>;
// Unknown position
constexpr Position kNonePos = std::numeric_limits<size_t>::max();

}  // namespace topa

#endif
