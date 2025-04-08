#include "simplex_tree.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <random>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace topa;

TEST(SimplexTreeTest, AddAndHasSimplex) {
    SimplexTree st(3);
    EXPECT_FALSE(st.has_simplex({0}));
    EXPECT_FALSE(st.has_simplex({0, 1}));

    st.add_simplex({0, 1}, 1.0);
    EXPECT_TRUE(st.has_simplex({0, 1}));
    EXPECT_FALSE(st.has_simplex({0}));
    EXPECT_FALSE(st.has_simplex({1}));
    EXPECT_FALSE(st.has_simplex({0, 2}));
}

TEST(SimplexTreeTest, AddSubsimplexAfterWithValidWeight) {
    SimplexTree st(3);
    st.add_simplex({0, 1}, 2.0);
    st.add_simplex({0}, 1.0);
    EXPECT_TRUE(st.has_simplex({0}));
    EXPECT_TRUE(st.has_simplex({0, 1}));
}

TEST(SimplexTreeTest, FacetsTest) {
    SimplexTree st(3);
    st.add_simplex({0}, 0.5);
    st.add_simplex({0, 1}, 1.0);
    st.add_simplex({0, 1, 2}, 2.0);

    auto facets = st.facets({0, 1, 2});
    ASSERT_EQ(facets.size(), 1u);
    EXPECT_EQ(facets[0].simplex, (SimplexTree::SortedSimplex {0, 1}));
    EXPECT_EQ(facets[0].weight, 1.0);
}

TEST(SimplexTreeTest, MultipleFacets) {
    SimplexTree st(3);
    st.add_simplex({0}, 0.5);
    st.add_simplex({1}, 0.5);
    st.add_simplex({0, 1}, 1.0);
    st.add_simplex({0, 2}, 1.0);
    st.add_simplex({1, 2}, 1.0);
    st.add_simplex({0, 1, 2}, 2.0);

    auto facets = st.facets({0, 1, 2});
    ASSERT_EQ(facets.size(), 3u);
    std::set<SimplexTree::SortedSimplex> expected = {
        {0, 1},
        {0, 2},
        {1, 2}
    };
    for (const auto& f : facets) {
        EXPECT_TRUE(expected.count(f.simplex));
    }
}

TEST(SimplexTreeTest, CofacetsTest) {
    SimplexTree st(4);
    st.add_simplex({0, 1}, 1.0);
    st.add_simplex({0, 1, 2}, 2.0);
    st.add_simplex({0, 1, 3}, 2.0);

    auto cofacets = st.cofacets({0, 1});
    ASSERT_EQ(cofacets.size(), 2u);
    std::vector<SimplexTree::SortedSimplex> expected = {
        {0, 1, 2},
        {0, 1, 3}
    };
    for (const auto& cf : cofacets) {
        EXPECT_TRUE(std::find(expected.begin(), expected.end(), cf.simplex) != expected.end());
    }
}

TEST(SimplexTreeTest, MultipleCofacets) {
    SimplexTree st(4);
    st.add_simplex({0}, 1.0);
    st.add_simplex({0, 1}, 2.0);
    st.add_simplex({0, 2}, 2.0);
    st.add_simplex({0, 3}, 2.0);

    auto cofacets = st.cofacets({0});
    ASSERT_EQ(cofacets.size(), 3u);
    std::set<SimplexTree::SortedSimplex> expected = {
        {0, 1},
        {0, 2},
        {0, 3}
    };
    for (const auto& cf : cofacets) {
        EXPECT_TRUE(expected.count(cf.simplex));
    }
}

TEST(SimplexTreeTest, NoExtraSubsimplices) {
    SimplexTree st(3);
    st.add_simplex({0, 1, 2}, 3.0);

    EXPECT_FALSE(st.has_simplex({0}));
    EXPECT_FALSE(st.has_simplex({0, 1}));
    EXPECT_FALSE(st.has_simplex({1, 2}));
}

TEST(SimplexTreeTest, EmptyFacetsAndCofacets) {
    SimplexTree st(3);
    st.add_simplex({0}, 1.0);

    auto facets = st.facets({0});
    EXPECT_TRUE(facets.empty());

    st.add_simplex({0, 1, 2}, 3.0);
    auto cofacets = st.cofacets({0, 1, 2});
    EXPECT_TRUE(cofacets.empty());
}

TEST(SimplexTreeTest, BoundaryWeights) {
    SimplexTree st(2);
    st.add_simplex({0}, std::numeric_limits<SimplexTree::Weight>::max());
    st.add_simplex({0, 1}, std::numeric_limits<SimplexTree::Weight>::max());
    EXPECT_TRUE(st.has_simplex({0, 1}));
}

TEST(SimplexTreeDeathTest, ReaddSimplex) {
    SimplexTree st(2);
    st.add_simplex({0, 1}, 1.0);
    st.add_simplex({0, 1}, 1.0);
    EXPECT_TRUE(st.has_simplex({0, 1}));

    EXPECT_DEBUG_DEATH(st.add_simplex({0, 1}, 0.5), ".*");
}

TEST(SimplexTreeDeathTest, WeightAssertionDeathTest) {
    SimplexTree st(3);
    st.add_simplex({0}, 1.0);
    EXPECT_DEBUG_DEATH(st.add_simplex({0, 1}, 0.5), ".*");
}

TEST(SimplexTreeDeathTest, UnsortedSimplex) {
    SimplexTree st(3);
    EXPECT_DEBUG_DEATH(st.add_simplex({1, 0}, 1.0), ".*");
    EXPECT_DEBUG_DEATH(st.has_simplex({1, 0}), ".*");
}

TEST(SimplexTreeStressTest, LargeTree) {
    const SimplexTree::VertexId N = 100;
    SimplexTree st(N);

    for (SimplexTree::VertexId i = 0; i < N; ++i) {
        st.add_simplex({i}, i + 1.0);
    }

    for (SimplexTree::VertexId i = 0; i < N; ++i) {
        for (SimplexTree::VertexId j = i + 1; j < N; ++j) {
            if ((i + j) % 5 == 0) {
                st.add_simplex({i, j}, std::max(i + 1.0, j + 1.0));
            }
        }
    }

    EXPECT_TRUE(st.has_simplex({0}));
    EXPECT_TRUE(st.has_simplex({N - 1}));
    EXPECT_TRUE(st.has_simplex({0, 5}));
}

TEST(SimplexTreeStressTest, DeepTree) {
    const int depth = 100;
    SimplexTree st(depth);

    SimplexTree::SortedSimplex simplex;
    for (SimplexTree::VertexId i = 0; i < depth; ++i) {
        simplex.push_back(i);
        st.add_simplex(simplex, i + 1.0);
    }

    auto facets = st.facets(simplex);
    ASSERT_EQ(facets.size(), 1u);
    SimplexTree::SortedSimplex expected_facet(simplex.begin(), simplex.end() - 1);
    EXPECT_EQ(facets[0].simplex, expected_facet);
}

TEST(SimplexTreeStressTest, WideTree) {
    const int width = 100;
    SimplexTree st(width + 1);
    st.add_simplex({0}, 0.0);

    for (SimplexTree::VertexId i = 1; i <= width; ++i) {
        st.add_simplex({0, i}, i * 1.0);
    }

    auto cofacets = st.cofacets({0});
    ASSERT_EQ(cofacets.size(), width);
}

TEST(SimplexTreeStressTest, ChaosMonkey) {
    const int num_operations = 1000;
    SimplexTree st(50);
    std::mt19937 rng(std::random_device {}());

    for (int i = 0; i < num_operations; ++i) {
        size_t size = std::uniform_int_distribution<>(1, 4)(rng);
        std::set<SimplexTree::VertexId> vertices;
        while (vertices.size() < size) {
            vertices.insert(std::uniform_int_distribution<>(0, 49)(rng));
        }
        SimplexTree::SortedSimplex simplex(vertices.begin(), vertices.end());
        SimplexTree::Weight weight = size * 100.0;

        st.add_simplex(simplex, weight);
        EXPECT_TRUE(st.has_simplex(simplex));
    }
}

TEST(SimplexTreeStressTest, RandomDependencyNetwork) {
    const int N = 10000;
    SimplexTree st(N);
    std::mt19937 rng(42);
    std::uniform_int_distribution<SimplexTree::VertexId> vertex_dist(0, N - 1);
    std::map<SimplexTree::SortedSimplex, SimplexTree::Weight> all_simplices;

    for (int i = 0; i < N; ++i) {
        SimplexTree::SortedSimplex s;
        for (int j = 0; j < 3; ++j) {
            s.insert(s.end(), vertex_dist(rng));
        }
        std::sort(s.begin(), s.end());
        s.erase(std::unique(s.begin(), s.end()), s.end());

        if (s.size() < 2) {
            continue;
        }

        SimplexTree::Weight max_subweight = 0.0;
        for (size_t j = 0; j < s.size(); ++j) {
            SimplexTree::SortedSimplex sub(s.begin(), s.begin() + j);
            sub.insert(sub.end(), s.begin() + j + 1, s.end());
            if (all_simplices.count(sub)) {
                max_subweight = std::max(max_subweight, all_simplices[sub]);
            }
        }

        SimplexTree::Weight w = max_subweight + 1.0 + rng() % 100;
        st.add_simplex(s, w);
        all_simplices[s] = w;
    }

    for (const auto& [s, w] : all_simplices) {
        if (s.size() > 1) {
            auto cofacets = st.cofacets(s);
            for (const auto& cf : cofacets) {
                EXPECT_TRUE(cf.simplex.size() == s.size() + 1);
            }
        }
    }
}

TEST(SimplexTreeStressTest, ExtremeSimplexSizes) {
    const int max_dim = 100;
    SimplexTree st(max_dim);
    SimplexTree::SortedSimplex s;

    for (SimplexTree::VertexId i = 0; i < max_dim; ++i) {
        s.push_back(i);
        st.add_simplex(s, i * 10.0);
    }
    for (int dim = 1; dim < max_dim; ++dim) {
        SimplexTree::SortedSimplex sub(s.begin(), s.begin() + dim);
        auto cofacets = st.cofacets(sub);
        ASSERT_EQ(cofacets.size(), 1u);
        EXPECT_EQ(cofacets[0].simplex.size(), dim + 1);
    }
}

class SimplexTreeFromTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        pointcloud_ = new Pointcloud(2);

        std::vector<std::vector<Pointcloud::CoordinateType>> points = {
            {0.0, 0.0},
            {1.0, 0.0},
            {0.0, 1.0}
        };

        for (const auto& p : points) {
            pointcloud_->add_point(p);
        }
    }

    static void TearDownTestSuite() {
        delete pointcloud_;
        pointcloud_ = nullptr;
    }

    static Pointcloud* pointcloud_;
};

Pointcloud* SimplexTreeFromTest::pointcloud_ = nullptr;

TEST_F(SimplexTreeFromTest, BuildsCorrectSimplices) {
    const auto max_radius = 1.5f;
    auto filtration = Filtration::VietorisRips(max_radius, 2);

    auto simplex_tree = SimplexTree::from(*pointcloud_, filtration);

    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(simplex_tree.has_simplex({i}));
    }

    const std::vector<std::pair<std::vector<SimplexTree::VertexId>, SimplexTree::Weight>> expected_edges = {
        {{0, 1}, 1.0f},
        {{0, 2}, 1.0f},
        {{1, 2}, 2.0f}
    };

    for (const auto& [edge, expected_weight] : expected_edges) {
        ASSERT_TRUE(simplex_tree.has_simplex(edge));
        EXPECT_FLOAT_EQ(simplex_tree.get_weight(edge), expected_weight);
    }

    const std::vector<SimplexTree::VertexId> triangle = {0, 1, 2};
    ASSERT_TRUE(simplex_tree.has_simplex(triangle));

    auto triangle_facets = simplex_tree.facets(triangle);
    ASSERT_EQ(triangle_facets.size(), 3);
    EXPECT_NEAR(triangle_facets[2].weight, 2.0f, 1e-5);
}

TEST_F(SimplexTreeFromTest, NoSimplicesWhenRadiusTooSmall) {
    auto filtration = Filtration::VietorisRips(0.5f, 2);
    auto simplex_tree = SimplexTree::from(*pointcloud_, filtration);

    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(simplex_tree.has_simplex({i}));
    }

    EXPECT_FALSE(simplex_tree.has_simplex({0, 1}));
    EXPECT_FALSE(simplex_tree.has_simplex({0, 2}));
    EXPECT_FALSE(simplex_tree.has_simplex({1, 2}));
    EXPECT_FALSE(simplex_tree.has_simplex({0, 1, 2}));
}
