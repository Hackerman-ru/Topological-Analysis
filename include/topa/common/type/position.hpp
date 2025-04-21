#pragma once

#include <limits>
#include <concepts>
#include <ranges>

namespace topa::common {

using Position = std::size_t;
constexpr Position kUnknownPos = std::numeric_limits<std::size_t>::max();

template <typename R>
concept PositionRange =
    std::ranges::input_range<R> &&
    std::convertible_to<std::ranges::range_value_t<R>, Position>;

}  // namespace topa::common
