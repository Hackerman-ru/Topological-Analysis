#include "topa/topa.hpp"

#include <catch2/catch_test_macros.hpp>
#include <random>
#include <set>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace topa;
using namespace topa::fast;

TEST_CASE("BitTreeHeap Core Functionality", "[bit-tree]") {
    const size_t N = 1000;
    BitTreeHeap col(N);

    SECTION("Add and get max") {
        col.Add({5, 10, 3});
        REQUIRE(col.GetMaxPos() == 10);

        col.Add(std::vector{15});
        REQUIRE(col.GetMaxPos() == 15);
    }

    SECTION("Persistent storage") {
        col.Add({7, 3, 5});
        auto copy = col;
        REQUIRE(copy.GetMaxPos() == 7);
    }

    SECTION("Reduction logic") {
        col.Add({10, 20, 30});
        REQUIRE(col.GetMaxPos() == 30);

        BitTreeHeap other(N);
        other.Add({30, 40});
        col += std::move(other);
        REQUIRE(col.GetMaxPos() == 40);
    }

    SECTION("Column clearing") {
        col.Add({42, 24, 100});
        col.Clear();
        REQUIRE(col.IsEmpty());
        REQUIRE(col.GetMaxPos() == kUnknownPos);
    }
}

TEST_CASE("Boundary Case Handling", "[bit-tree][edge]") {
    SECTION("Empty column behavior") {
        BitTreeHeap col(64);
        REQUIRE(col.GetMaxPos() == kUnknownPos);
        REQUIRE(col.IsEmpty());
    }

    SECTION("Single element column") {
        BitTreeHeap col(128);
        col.Add({127});
        REQUIRE(col.GetMaxPos() == 127);
        col.Add({127});
        REQUIRE(col.IsEmpty());
    }

    SECTION("Full block utilization") {
        BitTreeHeap col(64);
        col.Add({0, 63, 31});

        auto positions = col.PopAll();
        std::vector<Position> expected{0, 31, 63};
        REQUIRE(positions == expected);
    }
}

TEST_CASE("Stress Test with Algorithm Pattern", "[bit-tree][stress]") {
    const size_t N = 10000;
    BitTreeHeap col(N);
    std::mt19937 gen(42);
    std::uniform_int_distribution<Position> dist(0, N - 1);

    SECTION("Persistent-low simulation") {
        std::unordered_map<Position, BitTreeHeap> persistent_lows;

        for (int i = 0; i < 1000; ++i) {
            BitTreeHeap boundary(N);
            std::set<Position> simplex;

            while (simplex.size() < 5) {
                simplex.insert(dist(gen));
            }
            boundary.Add(simplex);

            while (!boundary.IsEmpty()) {
                auto pivot = boundary.GetMaxPos();
                if (persistent_lows.contains(pivot)) {
                    boundary += persistent_lows.at(pivot);
                } else {
                    persistent_lows.emplace(pivot, boundary);
                    break;
                }
            }
        }

        std::set<Position> unique_keys;
        for (const auto& [key, _] : persistent_lows) {
            unique_keys.insert(key);
        }
        REQUIRE(unique_keys.size() == persistent_lows.size());
    }

    SECTION("High-frequency updates") {
        for (int i = 0; i < 10000; ++i) {
            col.Add({dist(gen)});
        }
        REQUIRE(col.GetMaxPos() < N);

        for (int i = 0; i < 1000; ++i) {
            auto pivot = col.GetMaxPos();
            if (pivot == kUnknownPos)
                break;

            BitTreeHeap dummy(N);
            dummy.Add({pivot, pivot < N - 1 ? pivot + 1 : N - 1,
                       pivot > 0 ? pivot - 1 : 0});
            col += std::move(dummy);
        }
        Position max_pos = col.GetMaxPos();
        bool valid = max_pos < N || max_pos == kUnknownPos;
        REQUIRE(valid);
    }
}
