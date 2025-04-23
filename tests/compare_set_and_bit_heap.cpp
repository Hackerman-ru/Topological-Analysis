#include "common/set_heap.hpp"
#include "fast/bit_tree_heap.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <random>
#include <vector>
#include <algorithm>

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

namespace {

std::vector<Position> GenerateTestData(size_t size, Position max_pos) {
    std::vector<Position> data;
    data.reserve(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<Position> dist(0, max_pos);
    for (size_t i = 0; i < size; ++i) {
        data.push_back(dist(gen));
    }
    return data;
}

#define TEST(name) \
    TEMPLATE_TEST_CASE_SIG(name, "", ((size_t N), N), 100, 10000, 1000000)

}  // namespace

TEST("Benchmark Add Operation") {
    constexpr size_t data_size = N;
    constexpr Position max_pos = N;
    auto data = GenerateTestData(data_size, max_pos);

    BENCHMARK("SetHeap::Add") {
        SetHeap heap(0);
        heap.Add(data);
        return heap;
    };

    BENCHMARK("BitTreeHeap::Add") {
        BitTreeHeap heap(max_pos + 1);
        heap.Add(data);
        return heap;
    };
}

TEST("Benchmark GetMaxPos") {
    constexpr size_t data_size = N;
    constexpr Position max_pos = N;
    auto data = GenerateTestData(data_size, max_pos);

    SetHeap set_heap(0);
    set_heap.Add(data);
    BitTreeHeap bit_tree_heap(max_pos + 1);
    bit_tree_heap.Add(data);

    REQUIRE_FALSE(set_heap.IsEmpty());
    REQUIRE_FALSE(bit_tree_heap.IsEmpty());

    BENCHMARK("SetHeap::GetMaxPos") {
        return set_heap.GetMaxPos();
    };

    BENCHMARK("BitTreeHeap::GetMaxPos") {
        return bit_tree_heap.GetMaxPos();
    };
}

TEST("Benchmark PopMaxPos") {
    constexpr size_t data_size = N;
    constexpr Position max_pos = N;
    auto data = GenerateTestData(data_size, max_pos);

    BENCHMARK("SetHeap::PopMaxPos") {
        SetHeap heap(0);
        heap.Add(data);
        return heap.PopMaxPos();
    };

    BENCHMARK("BitTreeHeap::PopMaxPos") {
        BitTreeHeap heap(max_pos + 1);
        heap.Add(data);
        return heap.PopMaxPos();
    };
}

TEST("Benchmark PopAll") {
    constexpr size_t data_size = N;
    constexpr Position max_pos = N;
    auto data = GenerateTestData(data_size, max_pos);

    BENCHMARK("SetHeap::PopAll") {
        SetHeap heap(0);
        heap.Add(data);
        return heap.PopAll();
    };

    BENCHMARK("BitTreeHeap::PopAll") {
        BitTreeHeap heap(max_pos + 1);
        heap.Add(data);
        return heap.PopAll();
    };
}

TEST("Benchmark Operator +=") {
    constexpr size_t data_size = N;
    constexpr Position max_pos = N;
    auto data1 = GenerateTestData(data_size, max_pos);
    auto data2 = GenerateTestData(data_size, max_pos);

    SetHeap set_heap1(0);
    set_heap1.Add(data1);
    SetHeap set_heap2(0);
    set_heap2.Add(data2);

    BENCHMARK("SetHeap::operator +=") {
        SetHeap heap1 = set_heap1;
        heap1 += set_heap2;
        return heap1;
    };

    BitTreeHeap bit_heap1(max_pos + 1);
    bit_heap1.Add(data1);
    BitTreeHeap bit_heap2(max_pos + 1);
    bit_heap2.Add(data2);

    BENCHMARK("BitTreeHeap::operator +=") {
        BitTreeHeap heap1 = bit_heap1;
        heap1 += bit_heap2;
        return heap1;
    };
}
