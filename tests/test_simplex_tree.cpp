#include "simplex_tree.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>
#include <vector>
#include <algorithm>

using namespace topa;

TEST_CASE("SimplexTree Add and Has") {
    SimplexTree st(10);

    st.Add({0}, 0);
    st.Add({1, 2}, 1);
    st.Add({0, 1, 2}, 2);

    REQUIRE(st.Has({0}) == true);
    REQUIRE(st.Has({1, 2}) == true);
    REQUIRE(st.Has({0, 1, 2}) == true);
    REQUIRE(st.GetPosition({0}) == 0);
    REQUIRE(st.GetPosition({1, 2}) == 1);
    REQUIRE(st.GetPosition({0, 1, 2}) == 2);

    REQUIRE(st.Has({3}) == false);
    REQUIRE(st.Has({0, 2}) == false);
    REQUIRE(st.GetPosition({0, 2}) == SimplexTree::kNone);
}

TEST_CASE("GetFacets: Basic Functionality") {
    SimplexTree st(10);
    st.Add({0, 1, 2}, 100);
    st.Add({0, 1}, 10);
    st.Add({0, 2}, 20);
    st.Add({1, 2}, 30);
    st.Add({0}, 1);
    st.Add({1}, 2);
    st.Add({2}, 3);

    SECTION("Facets of 3-simplex") {
        auto facets = st.GetFacets({0, 1, 2});
        REQUIRE(facets.size() == 3);
        REQUIRE(std::find(facets.begin(), facets.end(), 10) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 20) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 30) != facets.end());
    }

    SECTION("Facets of 1-simplex (edge)") {
        auto facets = st.GetFacets({0, 1});
        REQUIRE(facets.size() == 2);
        REQUIRE(std::find(facets.begin(), facets.end(), 1) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 2) != facets.end());
    }

    SECTION("Facets of 0-simplex (vertex)") {
        auto facets = st.GetFacets({0});
        REQUIRE(facets.empty());  // У вершины нет граней
    }

    SECTION("Non-existent simplex") {
        auto facets = st.GetFacets({3, 4});
        REQUIRE(facets.empty());
    }
}

TEST_CASE("GetFacets: Exactly One Vertex Less") {
    SimplexTree st(10);

    st.Add({0, 1, 2}, 100);

    st.Add({0, 1}, 10);
    st.Add({0, 2}, 20);
    st.Add({1, 2}, 30);

    SECTION("Facets of 3-vertex simplex") {
        auto facets = st.GetFacets({0, 1, 2});
        REQUIRE(facets.size() == 3);
        REQUIRE(std::find(facets.begin(), facets.end(), 10) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 20) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 30) != facets.end());
    }

    SECTION("Facets of 2-vertex simplex (edge)") {
        st.Add({0}, 1);
        st.Add({1}, 2);
        auto facets = st.GetFacets({0, 1});
        REQUIRE(facets.size() == 2);
        REQUIRE(std::find(facets.begin(), facets.end(), 1) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 2) != facets.end());
    }
}

TEST_CASE("GetCofacets: Exactly One Vertex More") {
    SimplexTree st(10);

    st.Add({0, 1}, 10);

    st.Add({0, 1, 2}, 100);
    st.Add({0, 1, 3}, 101);

    SECTION("Cofacets of 2-vertex simplex") {
        auto cofacets = st.GetCofacets({0, 1});
        REQUIRE(cofacets.size() == 2);
        REQUIRE(std::find(cofacets.begin(), cofacets.end(), 100) !=
                cofacets.end());
        REQUIRE(std::find(cofacets.begin(), cofacets.end(), 101) !=
                cofacets.end());
    }

    SECTION("Cofacets of 3-vertex simplex (no cofacets)") {
        auto cofacets = st.GetCofacets({0, 1, 2});
        REQUIRE(cofacets.empty());
    }
}

TEST_CASE("GetFacets/Cofacets: After Move Semantics") {
    SimplexTree st1(10);
    st1.Add({0, 1, 2}, 100);
    st1.Add({0, 1}, 10);

    SimplexTree st2 = std::move(st1);
    REQUIRE(st2.GetFacets({0, 1, 2}).size() == 1);
    REQUIRE(st2.GetCofacets({0, 1}).size() == 1);
}

TEST_CASE("SimplexTree Move Semantics") {
    SimplexTree st1(10);
    st1.Add({0}, 0);
    st1.Add({0, 1}, 1);

    SimplexTree st2 = std::move(st1);
    REQUIRE(st2.Has({0}));
    REQUIRE(st2.Has({0, 1}));

    SimplexTree st3(10);
    st3 = std::move(st2);
    REQUIRE(st3.Has({0}));
    REQUIRE(st3.Has({0, 1}));
}

TEST_CASE("SimplexTree Cofacets With DifferentBranches") {
    SimplexTree st(10);
    st.Add({1, 2}, 0);
    st.Add({0, 1}, 2);
    st.Add({0, 1, 2}, 1);

    auto cofacets = st.GetCofacets({1, 2});
    REQUIRE(cofacets.size() == 1);
    REQUIRE(cofacets[0] == 1);

    cofacets = st.GetCofacets({0, 1});
    REQUIRE(cofacets.size() == 1);
    REQUIRE(cofacets[0] == 1);
}

TEST_CASE("Stress Test: Large SimplexTree") {
    SimplexTree st(1000);
    constexpr size_t kNumSimplices = 1000;

    for (size_t i = 0; i < kNumSimplices; ++i) {
        Simplex s;
        s.push_back(i % 100);
        if (i % 2 == 0)
            s.push_back((i + 1) % 100);
        std::sort(s.begin(), s.end());
        st.Add(s, i);
    }

    for (size_t i = 0; i < kNumSimplices; ++i) {
        Simplex s;
        s.push_back(i % 100);
        if (i % 2 == 0)
            s.push_back((i + 1) % 100);
        std::sort(s.begin(), s.end());
        REQUIRE(st.Has(s));
    }
}
