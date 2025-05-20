#include "topa/topa.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <vector>

using namespace topa::detail;

TEST_CASE("FlatMatrix Basic Functionality") {
    SECTION("Empty Matrix") {
        FlatMatrix<int> m(3);
        REQUIRE(m.size() == 0);
        REQUIRE(m.row_size() == 3);
        REQUIRE(m.begin() == m.end());
    }

    SECTION("Constructor with initial size") {
        FlatMatrix<int> m(3, 2);
        REQUIRE(m.size() == 2);
        REQUIRE(m.row_size() == 3);
        REQUIRE(m[0][0] == 0);
    }

    SECTION("Push back/pop back") {
        FlatMatrix<int> m(3);
        m.push_back({1, 2, 3});
        REQUIRE(m.size() == 1);
        REQUIRE(m[0][0] == 1);
        REQUIRE(m[0][1] == 2);
        REQUIRE(m[0][2] == 3);

        m.push_back(std::vector<int>{4, 5, 6});
        REQUIRE(m.size() == 2);
        REQUIRE(m[1][0] == 4);

        m.pop_back();
        REQUIRE(m.size() == 1);
    }

    SECTION("Element access") {
        FlatMatrix<int> m(2);
        m.push_back({1, 2});
        m.push_back({3, 4});

        SECTION("Operator[]") {
            REQUIRE(m[0][0] == 1);
            REQUIRE(m[1][1] == 4);
        }

        SECTION("Front/back") {
            REQUIRE(m.front()[0] == 1);
            REQUIRE(m.back()[1] == 4);
        }

        SECTION("Modification") {
            m[1][0] = 10;
            REQUIRE(m[1][0] == 10);
        }
    }
}

TEST_CASE("FlatMatrix Iterators") {
    FlatMatrix<int> m(2);
    m.push_back({1, 2});
    m.push_back({3, 4});
    m.push_back({5, 6});

    SECTION("Range-based for loop") {
        int sum = 0;
        for (const auto& row : m) {
            sum += row[0];
        }
        REQUIRE(sum == 9);
    }

    SECTION("Iterator operations") {
        auto it = m.begin();
        REQUIRE((*it)[0] == 1);

        ++it;
        REQUIRE((*it)[1] == 4);

        it += 2;
        REQUIRE(it == m.end());

        --it;
        REQUIRE((*it)[0] == 5);
    }

    SECTION("Const iterators") {
        const auto& cm = m;
        int count = 0;
        for (auto it = cm.begin(); it != cm.end(); ++it) {
            ++count;
        }
        REQUIRE(count == 3);
    }
}

TEST_CASE("FlatMatrix Conversion and Size") {
    FlatMatrix<int> m(3);
    m.push_back({1, 2, 3});
    m.push_back({4, 5, 6});

    SECTION("Vector conversion") {
        std::vector<int> v = m[0];
        REQUIRE(v == std::vector<int>{1, 2, 3});
    }

    SECTION("Size management") {
        REQUIRE(m.size() == 2);
        m.clear();
        REQUIRE(m.size() == 0);
        REQUIRE(m.row_size() == 3);
    }
}
