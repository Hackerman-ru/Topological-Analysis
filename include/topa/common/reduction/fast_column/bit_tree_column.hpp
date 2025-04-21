#pragma once

#include "common/type/position.hpp"
#include "fast_column.hpp"

#include <vector>
#include <cstdint>

namespace topa::common::reduction {

class BitTreeColumn : public FastColumn<BitTreeColumn> {
    using Block = uint64_t;
    using Data = std::vector<Block>;

   public:
    explicit BitTreeColumn(size_t columns_number);

    Position GetMaxPos() const;
    bool IsEmpty() const;

    std::vector<Position> PopAll();

    BitTreeColumn& operator+=(const BitTreeColumn& other);
    BitTreeColumn& operator+=(BitTreeColumn&& other);
    BitTreeColumn& operator+=(std::initializer_list<Position> positions);

    friend BitTreeColumn operator+(const BitTreeColumn& lhs,
                                   const BitTreeColumn& rhs);
    friend BitTreeColumn operator+(BitTreeColumn&& lhs,
                                   const BitTreeColumn& rhs);
    friend BitTreeColumn operator+(const BitTreeColumn& lhs,
                                   BitTreeColumn&& rhs);
    friend BitTreeColumn operator+(BitTreeColumn&& lhs, BitTreeColumn&& rhs);

    template <PositionRange R>
    void Add(R&& range) {
        for (const auto& pos : range) {
            Xor(pos);
        }
    }

    void Add(std::initializer_list<Position> positions) {
        Add(std::views::all(positions));
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
