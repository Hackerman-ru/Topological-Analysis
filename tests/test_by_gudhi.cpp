#include <gudhi/Rips_complex.h>
#include <gudhi/Points_off_io.h>
#include <gudhi/Simplex_tree.h>
#include <gudhi/distance_functions.h>
#include <gudhi/Persistent_cohomology.h>

#include <common/pointcloud.hpp>
#include <fast/full_vr.hpp>
#include <fast/full_tree_opt.hpp>
#include <common/new_vr.hpp>
#include <fast/flat_tree.hpp>
#include <common/filtered_complex.hpp>
#include <fast/double_twist.hpp>
#include <fast/sparse_matrix.hpp>
#include <fast/bit_tree_heap.hpp>
#include <common/detail/pairs_to_filtration_values.hpp>
#include <common/eucledian_distance.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/catch_approx.hpp>

namespace {

constexpr int MAX_DIM = 2;

struct Simplex_tree_options {
    typedef Gudhi::linear_indexing_tag Indexing_tag;
    typedef int Vertex_handle;
    typedef float Filtration_value;
    typedef std::uint32_t Simplex_key;
    static const bool store_key = true;
    static const bool store_filtration = true;
    static const bool contiguous_vertices = false;
    static const bool link_nodes_by_label = false;
    static const bool stable_simplex_handles = false;
};

using GudhiReader = Gudhi::Points_off_reader<std::vector<float>>;
using GudhiSimplexTree = Gudhi::Simplex_tree<Simplex_tree_options>;
using GudhiRipsComplex = Gudhi::rips_complex::Rips_complex<float>;

auto load_pointcloud(const std::string& path) {
    auto pc = topa::common::Pointcloud::Load(path);
    REQUIRE(pc.has_value());
    return *pc;
}

template <typename T>
void compare_vectors(const std::vector<T>& a, const std::vector<T>& b) {
    REQUIRE(a.size() == b.size());
    REQUIRE(std::equal(a.begin(), a.end(), b.begin()));
}

const std::vector<std::tuple<std::string, size_t>> TEST_CLOUDS = {
    {"100", 100}, {"klein", 361}};

auto get_test_clouds() {
    return TEST_CLOUDS | std::views::transform([](const auto& pair) {
               const auto& [name, size] = pair;
               return std::make_tuple(name, size,
                                      DATA_DIR "/pointclouds/" + name + ".off");
           });
}

}  // namespace

TEST_CASE("Data loading consistency") {
    const auto [name, expected_size, path] =
        GENERATE(Catch::Generators::from_range(get_test_clouds()));

    DYNAMIC_SECTION("Cloud: " << name) {
        SECTION("Compare point clouds between GUDHI and Topa") {
            const auto gudhi_pc = GudhiReader(path).get_point_cloud();
            const auto topa_pc = load_pointcloud(path);

            REQUIRE(gudhi_pc.size() == topa_pc.Size());

            const auto& g_points = gudhi_pc;
            const auto& t_points = topa_pc.GetPoints();

            std::vector<std::vector<float>> transformed;
            for (const auto& p : t_points) {
                transformed.emplace_back(static_cast<std::vector<float>>(p));
            }

            REQUIRE(g_points == transformed);
        }
    }
}

TEST_CASE("VR complex construction") {
    const auto [name, expected_size, path] =
        GENERATE(from_range(get_test_clouds()));

    DYNAMIC_SECTION("Cloud: " << name) {
        const auto gudhi_pc = GudhiReader(path).get_point_cloud();
        const auto topa_pc = load_pointcloud(path);

        REQUIRE(gudhi_pc.size() == expected_size);
        REQUIRE(topa_pc.Size() == expected_size);

        SECTION("Simplex and filtration equivalence") {
            const auto threshold = GENERATE(0.0f, 0.5f, 1.0f, 1.5f,
                                            std::numeric_limits<float>::max());

            GudhiRipsComplex g_rips(gudhi_pc, threshold,
                                    Gudhi::Euclidean_distance());
            GudhiSimplexTree g_st;
            g_rips.create_complex(g_st, MAX_DIM);

            const auto t_complex =
                topa::common::FilteredComplex<topa::fast::FlatTree>::From(
                    topa_pc,
                    topa::common::NewVR<topa::common::EucledianDistance>(
                        threshold, MAX_DIM));

            using SimplexData = std::pair<std::vector<int>, float>;
            std::vector<SimplexData> gudhi_data, topa_data;

            for (const auto& simplex : g_st.complex_simplex_range()) {
                auto vertices = g_st.simplex_vertex_range(simplex);
                std::vector<int> sorted_v(vertices.begin(), vertices.end());
                std::ranges::sort(sorted_v);
                gudhi_data.emplace_back(std::move(sorted_v),
                                        g_st.filtration(simplex));
            }

            for (const auto& fsimplex : t_complex.GetFSimplices()) {
                auto vertices = fsimplex.GetSimplex();
                std::vector<int> sorted_v(vertices.begin(), vertices.end());
                std::ranges::sort(sorted_v);
                topa_data.emplace_back(std::move(sorted_v),
                                       fsimplex.GetFiltrationValue());
            }

            auto comparator = [](const SimplexData& a, const SimplexData& b) {
                return a.first < b.first;
            };

            std::ranges::sort(gudhi_data, comparator);
            std::ranges::sort(topa_data, comparator);

            REQUIRE(gudhi_data.size() == topa_data.size());

            for (size_t i = 0; i < gudhi_data.size(); ++i) {
                REQUIRE_THAT(gudhi_data[i].first,
                             Catch::Matchers::Equals(topa_data[i].first));

                REQUIRE(gudhi_data[i].second ==
                        Catch::Approx(topa_data[i].second));
            }
        }

        SECTION("Full 2-skeleton") {
            GudhiRipsComplex g_rips(gudhi_pc, std::numeric_limits<float>::max(),
                                    Gudhi::Euclidean_distance());
            GudhiSimplexTree g_st;
            g_rips.create_complex(g_st, MAX_DIM);

            const auto t_complex =
                topa::common::FilteredComplex<topa::fast::FlatTree>::From(
                    topa_pc,
                    topa::fast::FullVR<topa::common::EucledianDistance>());

            using SimplexData = std::pair<std::vector<int>, float>;
            std::vector<SimplexData> gudhi_data, topa_data;

            for (const auto& simplex : g_st.complex_simplex_range()) {
                auto vertices = g_st.simplex_vertex_range(simplex);
                std::vector<int> sorted_v(vertices.begin(), vertices.end());
                std::ranges::sort(sorted_v);
                gudhi_data.emplace_back(std::move(sorted_v),
                                        g_st.filtration(simplex));
            }

            for (const auto& fsimplex : t_complex.GetFSimplices()) {
                auto vertices = fsimplex.GetSimplex();
                std::vector<int> sorted_v(vertices.begin(), vertices.end());
                std::ranges::sort(sorted_v);
                topa_data.emplace_back(std::move(sorted_v),
                                       fsimplex.GetFiltrationValue());
            }

            auto comparator = [](const SimplexData& a, const SimplexData& b) {
                return a.first < b.first;
            };

            std::ranges::sort(gudhi_data, comparator);
            std::ranges::sort(topa_data, comparator);

            REQUIRE(gudhi_data.size() == topa_data.size());

            for (size_t i = 0; i < gudhi_data.size(); ++i) {
                REQUIRE_THAT(gudhi_data[i].first,
                             Catch::Matchers::Equals(topa_data[i].first));

                REQUIRE(gudhi_data[i].second ==
                        Catch::Approx(topa_data[i].second));
            }
        }
    }
}
