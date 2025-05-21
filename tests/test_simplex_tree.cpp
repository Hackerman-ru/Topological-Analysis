#include "topa/topa.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>
#include <vector>
#include <algorithm>

using namespace topa;
using namespace topa::common;

TEST_CASE("SimplexTree Add and Has") {
    SimplexTree st(10);

    st.Add({0}, 0);
    st.Add({1, 2}, 1);
    st.Add({0, 1, 2}, 2);

    REQUIRE(st.Has({0}) == true);
    REQUIRE(st.Has({1, 2}) == true);
    REQUIRE(st.Has({0, 1, 2}) == true);
    REQUIRE(st.GetPos({0}) == 0);
    REQUIRE(st.GetPos({1, 2}) == 1);
    REQUIRE(st.GetPos({0, 1, 2}) == 2);

    REQUIRE(st.Has({3}) == false);
    REQUIRE(st.Has({0, 2}) == false);
    REQUIRE(st.GetPos({0, 2}) == kUnknownPos);
}

TEST_CASE("GetFacetsPos: Basic Functionality") {
    SimplexTree st(10);
    st.Add({0, 1, 2}, 100);
    st.Add({0, 1}, 10);
    st.Add({0, 2}, 20);
    st.Add({1, 2}, 30);
    st.Add({0}, 1);
    st.Add({1}, 2);
    st.Add({2}, 3);

    SECTION("Facets of 3-simplex") {
        auto facets = st.GetFacetsPos({0, 1, 2});
        REQUIRE(facets.size() == 3);
        REQUIRE(std::find(facets.begin(), facets.end(), 10) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 20) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 30) != facets.end());
    }

    SECTION("Facets of 1-simplex (edge)") {
        auto facets = st.GetFacetsPos({0, 1});
        REQUIRE(facets.size() == 2);
        REQUIRE(std::find(facets.begin(), facets.end(), 1) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 2) != facets.end());
    }

    SECTION("Facets of 0-simplex (vertex)") {
        auto facets = st.GetFacetsPos({0});
        REQUIRE(facets.empty());
    }

    SECTION("Non-existent simplex") {
        auto facets = st.GetFacetsPos({3, 4});
        REQUIRE(facets.empty());
    }
}

TEST_CASE("GetFacetsPos: Exactly One Vertex Less") {
    SimplexTree st(10);

    st.Add({0, 1, 2}, 100);

    st.Add({0, 1}, 10);
    st.Add({0, 2}, 20);
    st.Add({1, 2}, 30);

    SECTION("Facets of 3-vertex simplex") {
        auto facets = st.GetFacetsPos({0, 1, 2});
        REQUIRE(facets.size() == 3);
        REQUIRE(std::find(facets.begin(), facets.end(), 10) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 20) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 30) != facets.end());
    }

    SECTION("Facets of 2-vertex simplex (edge)") {
        st.Add({0}, 1);
        st.Add({1}, 2);
        auto facets = st.GetFacetsPos({0, 1});
        REQUIRE(facets.size() == 2);
        REQUIRE(std::find(facets.begin(), facets.end(), 1) != facets.end());
        REQUIRE(std::find(facets.begin(), facets.end(), 2) != facets.end());
    }
}

TEST_CASE("GetCofacetsPos: Exactly One Vertex More") {
    SimplexTree st(10);

    st.Add({0, 1}, 10);

    st.Add({0, 1, 2}, 100);
    st.Add({0, 1, 3}, 101);

    SECTION("Cofacets of 2-vertex simplex") {
        auto cofacets = st.GetCofacetsPos({0, 1});
        REQUIRE(cofacets.size() == 2);
        REQUIRE(std::find(cofacets.begin(), cofacets.end(), 100) !=
                cofacets.end());
        REQUIRE(std::find(cofacets.begin(), cofacets.end(), 101) !=
                cofacets.end());
    }

    SECTION("Cofacets of 3-vertex simplex (no cofacets)") {
        auto cofacets = st.GetCofacetsPos({0, 1, 2});
        REQUIRE(cofacets.empty());
    }
}

TEST_CASE("GetFacetsPos/Cofacets: After Move Semantics") {
    SimplexTree st1(10);
    st1.Add({0, 1, 2}, 100);
    st1.Add({0, 1}, 10);

    SimplexTree st2 = std::move(st1);
    REQUIRE(st2.GetFacetsPos({0, 1, 2}).size() == 1);
    REQUIRE(st2.GetCofacetsPos({0, 1}).size() == 1);
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

    auto cofacets = st.GetCofacetsPos({1, 2});
    REQUIRE(cofacets.size() == 1);
    REQUIRE(cofacets[0] == 1);

    cofacets = st.GetCofacetsPos({0, 1});
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

using Catch::Matchers::UnorderedRangeEquals;

TEST_CASE("SimplexTree basic operations") {
    SimplexTree st(5);

    SECTION("Add and check simplex existence") {
        st.Add({0}, 0);
        REQUIRE(st.Has({0}));
        REQUIRE(st.GetPos({0}) == 0);

        st.Add({0, 1}, 1);
        REQUIRE(st.Has({0, 1}));
        REQUIRE(st.GetPos({0, 1}) == 1);
    }

    SECTION("Facets retrieval") {
        st.Add({0}, 0);
        st.Add({1}, 1);
        st.Add({2}, 2);
        st.Add({0, 1}, 3);
        st.Add({0, 2}, 4);
        st.Add({1, 2}, 5);
        st.Add({0, 1, 2}, 6);

        auto facets = st.GetFacetsPos({0, 1, 2});
        REQUIRE(facets.size() == 3);
        std::vector<Position> expected = {3, 4, 5};
        REQUIRE_THAT(facets, UnorderedRangeEquals(expected));
    }

    SECTION("Cofacets retrieval with different hints") {
        st.Add({0, 1}, 1);
        st.Add({0, 1, 2}, 2);
        st.Add({0, 1, 3}, 3);
        st.Add({0, 2}, 4);

        SECTION("Sparse hint") {
            auto cofacets = st.GetCofacetsPos({0, 1});
            std::vector<Position> expected = {2, 3};
            REQUIRE_THAT(cofacets, UnorderedRangeEquals(expected));
        }

        SECTION("Dense hint") {
            auto cofacets = st.GetCofacetsPos({0, 1});
            std::vector<Position> expected = {2, 3};
            REQUIRE_THAT(cofacets, UnorderedRangeEquals(expected));
        }
    }

    SECTION("Non-existing simplex handling") {
        REQUIRE_FALSE(st.Has({0, 3}));
        REQUIRE(st.GetFacetsPos({0, 3}).empty());
        REQUIRE(st.GetCofacetsPos({0, 3}).empty());
    }

    SECTION("Boundary cases") {
        SECTION("Vertex cofacets") {
            st.Add({0}, 0);
            st.Add({0, 1}, 1);
            st.Add({0, 2}, 2);

            auto cofacets = st.GetCofacetsPos({});
            std::vector<Position> expected = {0, 1, 2};
        }

        SECTION("Facets of a vertex") {
            st.Add({0}, 0);
            REQUIRE(st.GetFacetsPos({0}).empty());
        }
    }
}
