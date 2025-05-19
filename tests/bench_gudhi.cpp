// #include <catch2/benchmark/catch_benchmark.hpp>
// #include <catch2/catch_test_macros.hpp>

// #include <gudhi/Rips_complex.h>
// #include <gudhi/Points_off_io.h>
// #include <gudhi/Simplex_tree.h>
// #include <gudhi/distance_functions.h>
// #include <gudhi/Matrix.h>
// #include <gudhi/persistence_matrix_options.h>

// using Point = std::vector<float>;
// using Simplex_tree = Gudhi::Simplex_tree<>;
// using Filtration_value = Simplex_tree::Filtration_value;
// using Rips_complex = Gudhi::rips_complex::Rips_complex<Filtration_value>;
// using Gudhi::persistence_matrix::Column_types;
// using Gudhi::persistence_matrix::Default_options;

// struct RU_options : Default_options<Column_types::INTRUSIVE_LIST, true> {
//     static const bool has_column_pairings = true;
//     static const bool can_retrieve_representative_cycles = true;
// };

// using RU_matrix = Gudhi::persistence_matrix::Matrix<RU_options>;

// void ComputePairs(std::string path) {
//     double threshold = 0.5f;
//     Gudhi::Points_off_reader<Point> off_reader(path);
//     Rips_complex rips_complex_from_file(off_reader.get_point_cloud(),
//     threshold,
//                                         Gudhi::Euclidean_distance());
//     Simplex_tree st;
//     rips_complex_from_file.create_complex(st, 2);
//     auto size = st.num_simplices();
//     RU_matrix matrix(size);
//     unsigned int id = 0;
//     for (auto sh : st.filtration_simplex_range()) {
//         st.assign_key(sh, id++);
//         std::vector<unsigned int> boundary;
//         for (auto b : st.boundary_simplex_range(sh)) {
//             boundary.push_back(st.key(b));
//         }
//         std::sort(boundary.begin(), boundary.end());
//         matrix.insert_boundary(boundary);
//     }
//     matrix.get_representative_cycles();
// }

// void ComputeFullPairs(std::string path) {
//     double threshold = std::numeric_limits<double>::infinity();
//     Gudhi::Points_off_reader<Point> off_reader(path);
//     Rips_complex rips_complex_from_file(off_reader.get_point_cloud(),
//     threshold,
//                                         Gudhi::Euclidean_distance());
//     Simplex_tree st;
//     rips_complex_from_file.create_complex(st, 2);
//     auto size = st.num_simplices();
//     RU_matrix matrix(size);
//     unsigned int id = 0;
//     for (auto sh : st.filtration_simplex_range()) {
//         st.assign_key(sh, id++);
//         std::vector<unsigned int> boundary;
//         for (auto b : st.boundary_simplex_range(sh)) {
//             boundary.push_back(st.key(b));
//         }
//         std::sort(boundary.begin(), boundary.end());
//         matrix.insert_boundary(boundary);
//     }
//     matrix.get_representative_cycles();
// }

// TEST_CASE("Bench peristence diagram computation") {
//     SECTION("VR") {
//         BENCHMARK("100 points") {
//             ComputePairs(DATA_DIR "/pointclouds/random_100.off");
//         };
//         BENCHMARK("200 points") {
//             ComputePairs(DATA_DIR "/pointclouds/random_200.off");
//         };
//         BENCHMARK("300 points") {
//             ComputePairs(DATA_DIR "/pointclouds/random_300.off");
//         };
//         BENCHMARK("400 points") {
//             ComputePairs(DATA_DIR "/pointclouds/random_400.off");
//         };
//         BENCHMARK("500 points") {
//             ComputePairs(DATA_DIR "/pointclouds/random_500.off");
//         };
//     }
//     SECTION("Full-VR") {
//         BENCHMARK("100 points") {
//             ComputeFullPairs(DATA_DIR "/pointclouds/random_100.off");
//         };
//         BENCHMARK("200 points") {
//             ComputeFullPairs(DATA_DIR "/pointclouds/random_200.off");
//         };
//         BENCHMARK("300 points") {
//             ComputeFullPairs(DATA_DIR "/pointclouds/random_300.off");
//         };
//         BENCHMARK("400 points") {
//             ComputeFullPairs(DATA_DIR "/pointclouds/random_400.off");
//         };
//         BENCHMARK("500 points") {
//             ComputeFullPairs(DATA_DIR "/pointclouds/random_500.off");
//         };
//     }
// }
