#ifndef TOPA_BIT_TREE_INDEX_COLUMN_HPP_
#define TOPA_BIT_TREE_INDEX_COLUMN_HPP_

#include "basic_types.hpp"
#include "position.hpp"

namespace topa::detail {

class BitTreeColumn {
    using Block = uint64_t;
    using Data = basic_types::DefaultContainer<Block>;

   public:
    using PositionsInitializerList = std::initializer_list<Position>;
    using PositionsView = basic_types::DefaultView<Position>;
    using Positions = basic_types::DefaultContainer<Position>;

   public:
    explicit BitTreeColumn(size_t columns_number);

    friend BitTreeColumn operator+(const BitTreeColumn& lhs,
                                   const BitTreeColumn& rhs);
    friend BitTreeColumn operator+(BitTreeColumn&& lhs,
                                   const BitTreeColumn& rhs);
    friend BitTreeColumn operator+(const BitTreeColumn& lhs,
                                   BitTreeColumn&& rhs);
    friend BitTreeColumn operator+(BitTreeColumn&& lhs, BitTreeColumn&& rhs);

    Position GetMaxPos() const;

    BitTreeColumn& operator+=(const BitTreeColumn& other);
    BitTreeColumn& operator+=(const PositionsInitializerList& positions);
    BitTreeColumn& operator+=(const PositionsView& positions);

    template <typename Container>
    BitTreeColumn& Add(const Container& positions) {
        for (const auto& pos : positions) {
            Xor(pos);
        }
        return *this;
    }

    bool Empty() const;

    explicit operator Positions() const;

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

}  // namespace topa::detail

#endif
