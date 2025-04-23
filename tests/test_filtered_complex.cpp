#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "common/filtered_complex.hpp"

#include <map>

using namespace topa;
using namespace topa::common;

struct MockSimplexTreeImpl : models::SimplexTree<MockSimplexTreeImpl> {
    using Simplex = std::vector<topa::VertexId>;
    using Position = topa::Position;

    explicit MockSimplexTreeImpl(std::size_t) {
    }

    struct SimplexCompare {
        bool operator()(const Simplex& a, const Simplex& b) const {
            if (a.size() != b.size())
                return a.size() < b.size();
            return a < b;
        }
    };

    std::map<Simplex, Position, SimplexCompare> simplices;
    std::map<Position, Simplex> pos_to_simplex;

    template <typename R>
    void Add(R&& simplex, Position pos) {
        Simplex s(std::begin(simplex), std::end(simplex));
        std::sort(s.begin(), s.end());
        simplices[s] = pos;
        pos_to_simplex[pos] = s;
    }

    template <typename R>
    bool Has(R&& simplex) const {
        Simplex s(std::begin(simplex), std::end(simplex));
        std::sort(s.begin(), s.end());
        return simplices.count(s);
    }

    template <typename R>
    Position GetPos(R&& simplex) const {
        Simplex s(std::begin(simplex), std::end(simplex));
        std::sort(s.begin(), s.end());
        return simplices.at(s);
    }

    template <typename R>
    std::vector<Position> GetFacetsPos(R&& simplex) const {
        Simplex s(std::begin(simplex), std::end(simplex));
        std::sort(s.begin(), s.end());

        std::vector<Position> facets;
        for (size_t i = 0; i < s.size(); ++i) {
            Simplex facet;
            for (size_t j = 0; j < s.size(); ++j) {
                if (j != i)
                    facet.push_back(s[j]);
            }
            if (simplices.count(facet)) {
                facets.push_back(simplices.at(facet));
            }
        }
        return facets;
    }

    template <typename R>
    std::vector<Position> GetCofacetsPos(R&& simplex) const {
        Simplex s(std::begin(simplex), std::end(simplex));
        std::sort(s.begin(), s.end());

        std::vector<Position> cofacets;
        for (const auto& [cof_simplex, pos] : simplices) {
            if (cof_simplex.size() == s.size() + 1 &&
                std::includes(cof_simplex.begin(), cof_simplex.end(), s.begin(),
                              s.end())) {
                cofacets.push_back(pos);
            }
        }
        return cofacets;
    }
};

// Мок Filtration для тестирования
struct MockFiltration : topa::models::Filtration<MockFiltration> {
    template <typename Cloud>
    topa::models::Filtration<MockFiltration>::WSimplices Filter(
        const Cloud&) const {
        return {
            topa::WSimplex({1}, 0.2),       // Вершина 1
            topa::WSimplex({2}, 0.2),       // Вершина 2
            topa::WSimplex({1, 2}, 0.2),    // Ребро 1-2
            topa::WSimplex({0}, 0.3),       // Вершина 0
            topa::WSimplex({0, 2}, 0.5),    // Ребро 0-2
            topa::WSimplex({0, 1}, 1.0),    // Ребро 0-1
            topa::WSimplex({0, 1, 2}, 1.0)  // 2-симплекс
        };
    }
};

struct MockPointcloudTraits {
    using Distance = double;
    using Point = int;
    using Points = std::vector<Point>;
};

struct MockPointcloud
    : topa::models::Pointcloud<MockPointcloud, MockPointcloudTraits> {
    std::size_t Size() const {
        return 3;
    }
    const Points& GetPoints() const {
        static Points pts{1, 2, 3};
        return pts;
    }
    Distance GetDistance(std::size_t, std::size_t) const {
        return 0;
    }
};

TEST_CASE("FilteredComplex construction", "[FilteredComplex]") {
    MockPointcloud cloud;
    MockFiltration filtration;

    auto fc = FilteredComplex<MockSimplexTreeImpl>::From(cloud, filtration);

    SECTION("Check total simplices count") {
        REQUIRE(fc.GetPosesBySize(1).size() == 3);  // 3 вершины
        REQUIRE(fc.GetPosesBySize(2).size() == 3);  // 3 ребра
        REQUIRE(fc.GetPosesBySize(3).size() == 1);  // 1 2-симплекс
    }
}

TEST_CASE("FilteredComplex facets", "[FilteredComplex]") {
    MockPointcloud cloud;
    MockFiltration filtration;
    auto fc = FilteredComplex<MockSimplexTreeImpl>::From(cloud, filtration);

    SECTION("Facets for edge [0,1]") {
        Position edge_pos = 5;  // Позиция ребра 0-1 в wsimplices_
        auto facets = fc.GetFacetsPosition(edge_pos);

        REQUIRE(facets.size() == 2);
        REQUIRE_THAT(facets,
                     Catch::Matchers::UnorderedEquals(topa::Simplex{0, 3}));
    }

    SECTION("Facets for triangle [0,1,2]") {
        Position triangle_pos = 6;
        auto facets = fc.GetFacetsPosition(triangle_pos);

        REQUIRE(facets.size() == 3);
        REQUIRE_THAT(facets,
                     Catch::Matchers::UnorderedEquals(topa::Simplex{2, 4, 5}));
    }
}

TEST_CASE("FilteredComplex cofacets", "[FilteredComplex]") {
    MockPointcloud cloud;
    MockFiltration filtration;
    auto fc = FilteredComplex<MockSimplexTreeImpl>::From(cloud, filtration);

    SECTION("Cofacets for vertex 0") {
        Position vertex_pos = 3;
        auto cofacets = fc.GetCofacetsPosition(vertex_pos);

        REQUIRE(cofacets.size() == 2);
        REQUIRE_THAT(cofacets,
                     Catch::Matchers::UnorderedEquals(topa::Simplex{4, 5}));
    }

    SECTION("Cofacets for edge [0,1]") {
        Position edge_pos = 5;
        auto cofacets = fc.GetCofacetsPosition(edge_pos);

        REQUIRE(cofacets.size() == 1);
        REQUIRE(cofacets[0] == 6);  // Треугольник [0,1,2]
    }
}
