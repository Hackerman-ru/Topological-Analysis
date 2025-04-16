#include "detail/bit_tree_column.hpp"

#include <catch2/catch_test_macros.hpp>
#include <random>
#include <set>
#include <unordered_map>

using namespace topa;

TEST_CASE("BitTreeColumn Core Functionality", "[bit-tree]") {
    const size_t N = 1000;
    detail::BitTreeColumn col(N);

    SECTION("Add and get max") {
        col += {5, 10, 3};
        REQUIRE(col.GetMaxPos() == 10);

        col += {15};
        REQUIRE(col.GetMaxPos() == 15);
    }

    SECTION("Persistent storage") {
        col += {7, 3, 5};
        auto copy = col;
        REQUIRE(copy.GetMaxPos() == 7);
    }

    SECTION("Reduction logic") {
        col += {10, 20, 30};
        REQUIRE(col.GetMaxPos() == 30);

        detail::BitTreeColumn other(N);
        other += {30, 40};
        col += other;
        REQUIRE(col.GetMaxPos() == 40);
    }

    SECTION("Column clearing") {
        col += {42, 24, 100};
        col.Clear();
        REQUIRE(col.Empty());
        REQUIRE(col.GetMaxPos() == kNonePos);
    }
}

TEST_CASE("Boundary Case Handling", "[bit-tree][edge]") {
    SECTION("Empty column behavior") {
        detail::BitTreeColumn col(64);
        REQUIRE(col.GetMaxPos() == kNonePos);
        REQUIRE(col.PopAll().empty());
    }

    SECTION("Single element column") {
        detail::BitTreeColumn col(128);
        col += {127};
        REQUIRE(col.GetMaxPos() == 127);
        col += {127};
        REQUIRE(col.Empty());
    }

    SECTION("Full block utilization") {
        detail::BitTreeColumn col(64);
        col += {0, 63, 31};
        auto positions = col.PopAll();
        REQUIRE(positions == std::vector<Position>{0, 31, 63});
    }
}

TEST_CASE("Stress Test with Algorithm Pattern", "[bit-tree][stress]") {
    const size_t N = 10000;
    detail::BitTreeColumn col(N);
    std::mt19937 gen(42);
    std::uniform_int_distribution<Position> dist(0, N - 1);

    SECTION("Persistent-low simulation") {
        std::unordered_map<Position, detail::BitTreeColumn> persistent_lows;

        for (int i = 0; i < 1000; ++i) {
            detail::BitTreeColumn boundary(N);
            std::set<Position> simplex;

            while (simplex.size() < 5) {
                simplex.insert(dist(gen));
            }
            boundary.Add(simplex);

            while (!boundary.Empty()) {
                auto pivot = boundary.GetMaxPos();
                if (persistent_lows.count(pivot)) {
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
            col += {dist(gen)};
        }
        REQUIRE(col.GetMaxPos() < N);

        for (int i = 0; i < 1000; ++i) {
            auto pivot = col.GetMaxPos();
            if (pivot == kNonePos)
                break;

            detail::BitTreeColumn dummy(N);
            dummy += {pivot, pivot < N - 1 ? pivot + 1 : N - 1,
                      pivot > 0 ? pivot - 1 : 0};
            col += dummy;
        }
        bool valid = col.GetMaxPos() < N || col.Empty();
        REQUIRE(valid);
    }
}