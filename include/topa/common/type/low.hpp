#pragma once

#include "position.hpp"

namespace topa {

using Low = Position;
constexpr Low kUnknownLow = kUnknownPos;

template <typename R>
concept LowRange = PositionRange<R>;

}  // namespace topa
