#pragma once

#include "models/pos_heap.hpp"

#include <vector>
#include <cstdint>

namespace topa::fast {

class BitTreeHeap : public models::PosHeap<BitTreeHeap> {
    using Block = uint64_t;
    using Data = std::vector<Block>;

   public:
    explicit BitTreeHeap(size_t columns_number);

    Position GetMaxPos() const;
    bool IsEmpty() const;

    BitTreeHeap& operator+=(const BitTreeHeap& other);
    BitTreeHeap& operator+=(BitTreeHeap&& other);
    BitTreeHeap& operator+=(std::initializer_list<Position> positions);

    friend BitTreeHeap operator+(const BitTreeHeap& lhs,
                                 const BitTreeHeap& rhs);
    friend BitTreeHeap operator+(BitTreeHeap&& lhs, const BitTreeHeap& rhs);
    friend BitTreeHeap operator+(const BitTreeHeap& lhs, BitTreeHeap&& rhs);
    friend BitTreeHeap operator+(BitTreeHeap&& lhs, BitTreeHeap&& rhs);

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
    Positions PopAll();
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

}  // namespace topa::fast
