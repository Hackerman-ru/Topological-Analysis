#include "complex.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace topa;

TEST_CASE("Harmonic Test") {
    auto pointcloud = Pointcloud::Load(DATA_DIR "/pointclouds/klein.off");
    Filtration filtration = Filtration::FullVietorisRips();
    Complex complex = Complex::From(*pointcloud, filtration);
    // TODO:
    // auto boundary_matrix = BoundaryMatrix::From(complex);
    // auto coboundary_matrix = CoboundaryMatrix::From(complex);
    // 5. Reducer reducer = Reducer::DoubleTwist();
    // 6. BoundaryMatrix reduced_boundary_matrix =
    // reducer.Reduce(boundary_matrix);
    // 7. auto representative_vectors =
    // Algorithms::ComputeRepresentativeVectors(reduced_boundary_matrix);
}
