#pragma once

#include <concepts>
#include <ranges>
#include <cstddef>

namespace topa {

using VertexId = std::size_t;
template <typename R>
concept VertexRange =
    std::ranges::contiguous_range<R> &&
    std::convertible_to<std::ranges::range_value_t<R>, VertexId>;

}  // namespace topa
