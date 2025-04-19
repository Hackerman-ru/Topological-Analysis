#pragma once

#include <limits>

namespace topa::common {

using Position = std::size_t;
constexpr Position kUnknown = std::numeric_limits<std::size_t>::max();

}  // namespace topa::common
