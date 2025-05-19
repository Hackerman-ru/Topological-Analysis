// #include <catch2/benchmark/catch_benchmark.hpp>
// #include <catch2/catch_test_macros.hpp>

// #include <dionysus/distances.h>
// #include <dionysus/rips.h>
// #include <dionysus/filtration.h>
// #include <dionysus/fields/zp.h>
// #include <dionysus/fields/z2.h>
// #include <dionysus/ordinary-persistence.h>
// #include <dionysus/cohomology-persistence.h>
// #include <dionysus/zigzag-persistence.h>
// #include <dionysus/standard-reduction.h>
// #include <dionysus/row-reduction.h>
// #include <dionysus/pair-recorder.h>
// namespace d = dionysus;

// #include <sstream>
// #include <fstream>

// template <class PointContainer>
// void read_points(const std::string& infilename, PointContainer& points) {
//     typedef typename PointContainer::value_type Point;

//     std::ifstream in(infilename.c_str());
//     std::string line;
//     std::getline(in, line);
//     std::getline(in, line);

//     while (std::getline(in, line)) {
//         std::stringstream linestream(line);
//         double x;
//         points.push_back(Point());
//         while (linestream >> x) points.back().push_back(x);
//     }
// }

// typedef std::vector<float> Point;
// typedef std::vector<Point> PointContainer;
// typedef d::PairwiseDistances<PointContainer, d::L2Distance<Point>>
//     PairDistances;
// typedef PairDistances::DistanceType DistanceType;
// typedef PairDistances::IndexType Vertex;
// typedef d::Rips<PairDistances> Generator;
// typedef Generator::Simplex Simplex;
// typedef d::Filtration<Simplex> Filtration;
// typedef d::Z2Field K;
// typedef d::PairRecorder<d::CohomologyPersistence<K>> Persistence;

// void ComputePairs(std::string path) {
//     short unsigned skeleton = 2;
//     DistanceType max_distance = 0.5f;

//     PointContainer points;
//     read_points(path, points);

//     PairDistances distances(points);

//     Generator rips(distances);
//     Generator::Evaluator size(distances);
//     Filtration filtration;

//     rips.generate(skeleton, max_distance, [&filtration](Simplex&& s) {
//         filtration.push_back(s);
//     });

//     filtration.sort(Generator::Comparison(distances));

//     K k;
//     Persistence persistence(k);
//     d::StandardReduction<Persistence> reduce(persistence);
//     reduce(filtration);
// }

// void ComputeFullPairs(std::string path) {
//     short unsigned skeleton = 2;
//     DistanceType max_distance =
//     std::numeric_limits<DistanceType>::infinity();

//     PointContainer points;
//     read_points(path, points);

//     PairDistances distances(points);

//     Generator rips(distances);
//     Generator::Evaluator size(distances);
//     Filtration filtration;

//     rips.generate(skeleton, max_distance, [&filtration](Simplex&& s) {
//         filtration.push_back(s);
//     });

//     filtration.sort(Generator::Comparison(distances));

//     K k;
//     Persistence persistence(k);
//     d::StandardReduction<Persistence> reduce(persistence);
//     reduce(filtration);
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
