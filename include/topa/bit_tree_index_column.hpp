#ifndef TOPA_BIT_TREE_INDEX_COLUMN_HPP
#define TOPA_BIT_TREE_INDEX_COLUMN_HPP

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

namespace topa {
    // Inspired by bit_tree_column class from PHAT library
    template<typename index_t = size_t>
    class BitTreeIndexColumn {
        using block_t = uint64_t;
        using Data = std::vector<block_t>;
        using Indices = std::vector<index_t>;

    public:
        BitTreeIndexColumn(index_t columns_number) {
            init(columns_number);
        }

        friend BitTreeIndexColumn operator+(const BitTreeIndexColumn& lhs, const BitTreeIndexColumn& rhs) {
            BitTreeIndexColumn result = lhs;
            result += rhs;
            return result;
        }

        friend BitTreeIndexColumn operator+(BitTreeIndexColumn&& lhs, const BitTreeIndexColumn& rhs) {
            lhs += rhs;
            return lhs;
        }

        friend BitTreeIndexColumn operator+(const BitTreeIndexColumn& lhs, BitTreeIndexColumn&& rhs) {
            rhs += lhs;
            return rhs;
        }

        friend BitTreeIndexColumn operator+(BitTreeIndexColumn&& lhs, BitTreeIndexColumn&& rhs) {
            lhs += rhs;
            return lhs;
        }

        index_t get_max_index() const {
            if (empty()) {
                return none;
            }

            size_t node = 0;
            size_t next_node = 0;
            size_t index = 0;

            while (next_node < m_data.size()) {
                node = next_node;
                index = rightmost_bit(m_data[node]);
                next_node = (node << block_shift) + index + 1;
            }

            return ((node - m_offset) << block_shift) + index;
        }

        template<typename InputIterator>
        void add(InputIterator begin, InputIterator end) {
            for (InputIterator it = begin; it != end; ++it) {
                xor_index(*it);
            }
        }

        template<typename Container>
        void add(const Container& indices) {
            for (const auto& index : indices) {
                xor_index(index);
            }
        }

        bool empty() const {
            return m_data[0] == 0;
        }

        template<typename Container>
        operator Container() const {   // should be const
            Indices indices = get_indices_and_clear();
            add(indices);
            return Container(std::move(indices));
        }

        Indices get_indices_and_clear() {
            Indices indices;
            index_t max_index = get_max_index();
            while (max_index != static_cast<index_t>(-1)) {
                indices.emplace_back(std::move(max_index));
                xor_index(max_index);
                max_index = get_max_index();
            }
            std::reverse(indices.begin(), indices.end());
            return indices;
        }

        void clear() {
            index_t max_index = get_max_index();
            while (max_index != none) {
                xor_index(max_index);
                max_index = get_max_index();
            }
        }

    private:
        void init(size_t columns_number) {
            const size_t bottom_nodes = (columns_number + bits_in_block - 1) / bits_in_block;
            size_t prebottom_nodes_number = 1;

            while (prebottom_nodes_number * bits_in_block < bottom_nodes) {
                prebottom_nodes_number *= bits_in_block;
                m_offset += prebottom_nodes_number;
            }

            m_data.resize(m_offset + bottom_nodes, 0);
        }

        // Some magic: http://graphics.stanford.edu/~seander/bithacks.html
        // Gets the position of the rightmost bit of 'x'. 0 means the most significant bit.
        // (-x)&x isolates the rightmost bit.
        // The whole method is much faster than calling log2i, and very comparable to using ScanBitForward/Reverse intrinsic,
        // which should be one CPU instruction, but is not portable.
        index_t rightmost_bit(const block_t& block) const {
            return static_cast<index_t>(63)
                 - magic_table[((block & (-(int64_t)block)) * 0x07EDD5E59A4E28C2) >> 58];
        }

        void xor_index(index_t index) {
            static constexpr block_t one = 1;
            static constexpr block_t block_modulo_mask = (one << block_shift) - 1;
            size_t index_in_level = index >> block_shift;
            size_t address = index_in_level + m_offset;
            size_t index_in_block = index & block_modulo_mask;
            block_t mask = (one << (bits_in_block - index_in_block - 1));
            if (address >= m_data.size()) {
                std::cout << "Something wrong: " << address << '\n';
                exit(-1);
            }
            m_data[address] ^= mask;

            while (address && !(m_data[address] & ~mask)) {
                index_in_block = index_in_level & block_modulo_mask;
                index_in_level >>= block_shift;
                --address;
                address >>= block_shift;
                mask = (one << (bits_in_block - index_in_block - 1));
                if (address >= m_data.size()) {
                    std::cout << "Something wrong: " << address << '\n';
                    exit(-1);
                }
                m_data[address] ^= mask;
            }
        }

        static constexpr index_t none = -1;
        static constexpr size_t bits_in_block = 64;
        static constexpr size_t block_shift = 6;
        static constexpr index_t magic_table[64] = {
            63, 0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54, 33, 42, 3,  61, 51, 37, 40, 49, 18,
            28, 20, 55, 30, 34, 11, 43, 14, 22, 4,  62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19,
            29, 10, 13, 21, 56, 45, 25, 31, 35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5};

        size_t m_offset = 1;
        Data m_data;
    };
}   // namespace topa

#endif
