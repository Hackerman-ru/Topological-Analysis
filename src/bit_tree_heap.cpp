#include "fast/bit_tree_heap.hpp"

#include <algorithm>
#include <cassert>

namespace topa::fast {

BitTreeHeap::BitTreeHeap(size_t n_cols) {
    Init(n_cols);
}

Position BitTreeHeap::GetMaxPos() const {
    if (IsEmpty()) {
        return kUnknownPos;
    }

    size_t node = 0;
    size_t next_node = 0;
    size_t index = 0;

    while (next_node < data_.size()) {
        node = next_node;
        index = RightmostBit(data_[node]);
        next_node = (node << kBlockShift) + index + 1;
    }

    return ((node - offset_) << kBlockShift) + index;
}

bool BitTreeHeap::IsEmpty() const {
    return data_[0] == 0;
}

BitTreeHeap& BitTreeHeap::operator+=(BitTreeHeap&& other) {
    for (const auto& pos : other.PopAll()) {
        Xor(pos);
    }
    return *this;
}

BitTreeHeap& BitTreeHeap::operator+=(const BitTreeHeap& other) {
    BitTreeHeap copy(other);
    for (const auto& pos : copy.PopAll()) {
        Xor(pos);
    }
    return *this;
}

BitTreeHeap& BitTreeHeap::operator+=(
    std::initializer_list<Position> positions) {
    for (const auto& pos : positions) {
        Xor(pos);
    }
    return *this;
}

BitTreeHeap operator+(const BitTreeHeap& lhs, const BitTreeHeap& rhs) {
    BitTreeHeap result = lhs;
    result += rhs;
    return result;
}

BitTreeHeap operator+(BitTreeHeap&& lhs, const BitTreeHeap& rhs) {
    lhs += rhs;
    return lhs;
}

BitTreeHeap operator+(const BitTreeHeap& lhs, BitTreeHeap&& rhs) {
    rhs += lhs;
    return rhs;
}

BitTreeHeap operator+(BitTreeHeap&& lhs, BitTreeHeap&& rhs) {
    lhs += std::move(rhs);
    return lhs;
}

Position BitTreeHeap::PopMaxPos() {
    Position max_pos = GetMaxPos();
    if (max_pos != kUnknownPos) {
        Xor(max_pos);
    }
    return max_pos;
}

BitTreeHeap::Positions BitTreeHeap::PopAll() {
    std::vector<Position> positions;
    Position max_pos = GetMaxPos();
    while (max_pos != kUnknownPos) {
        positions.emplace_back(max_pos);
        Xor(max_pos);
        max_pos = GetMaxPos();
    }
    std::reverse(positions.begin(), positions.end());
    return positions;
}

void BitTreeHeap::Clear() {
    Position max_pos = GetMaxPos();
    while (max_pos != kUnknownPos) {
        Xor(max_pos);
        max_pos = GetMaxPos();
    }
}

void BitTreeHeap::Init(size_t n_cols) {
    const size_t bottom_nodes = (n_cols + kBlockBits - 1) / kBlockBits;
    size_t prebottom_nodes_number = 1;

    while (prebottom_nodes_number * kBlockBits < bottom_nodes) {
        prebottom_nodes_number *= kBlockBits;
        offset_ += prebottom_nodes_number;
    }

    data_.resize(offset_ + bottom_nodes, 0);
}

Position BitTreeHeap::RightmostBit(const Block& block) const {
    static constexpr int kDeBruijn64[64] = {
        63, 0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54, 33, 42, 3,
        61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4,
        62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21,
        56, 45, 25, 31, 35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5};

    return static_cast<Position>(63) -
           kDeBruijn64[((block & (-static_cast<int64_t>(block))) *
                        0x07EDD5E59A4E28C2) >>
                       58];
}

void BitTreeHeap::Xor(Position pos) {
    static constexpr Block kOne = 1;
    static constexpr Block kBlockModuloMask = (kOne << kBlockShift) - 1;
    size_t index_in_level = pos >> kBlockShift;
    size_t address = index_in_level + offset_;
    size_t index_in_block = pos & kBlockModuloMask;
    Block mask = (kOne << (kBlockBits - index_in_block - 1));
    assert(address < data_.size());
    data_[address] ^= mask;

    while (address != 0 && (data_[address] & ~mask) == 0) {
        index_in_block = index_in_level & kBlockModuloMask;
        index_in_level >>= kBlockShift;
        --address;
        address >>= kBlockShift;
        mask = (kOne << (kBlockBits - index_in_block - 1));
        assert(address < data_.size());
        data_[address] ^= mask;
    }
}

}  // namespace topa::fast
