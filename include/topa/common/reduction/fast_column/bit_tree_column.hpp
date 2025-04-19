#pragma once

#include "common/type/position.hpp"

#include <ranges>
#include <vector>
#include <cstdint>

namespace topa::common::reduction {

class BitTreeColumn {
    using Block = uint64_t;
    using Data = std::vector<Block>;

   public:
    explicit BitTreeColumn(size_t columns_number);

    Position GetMaxPos() const;
    bool Empty() const;

    template <std::ranges::input_range R>
    BitTreeColumn& Add(R&& range) {
        for (const auto& pos : range) {
            Xor(pos);
        }
        return *this;
    }

    Position PopMaxPos();
    void Clear();

   private:
    void Init(size_t columns_number);
    Position RightmostBit(const Block& block) const;
    void Xor(Position index);

    static constexpr size_t kBlockBits = 64;
    static constexpr size_t kBlockShift = 6;

    size_t offset_ = 1;
    Data data_;
};

}  // namespace topa::common::reduction
