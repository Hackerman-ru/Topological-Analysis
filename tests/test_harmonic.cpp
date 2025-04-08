#include "complex.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace topa;

TEST_CASE("Harmonic Test") {
    Pointcloud pointcloud =
        *Pointcloud::Load(DATA_DIR "/pointclouds/klein.off");
    Filtration filtration = Filtration::VietorisRips();
    Complex complex = Complex::From(pointcloud, filtration);
    // TODO:
    // 4. BoundaryMatrix boundary_matrix = BoundaryMatrix::from(simplex_tree);
    // 5. Reducer reducer = Reducer::DoubleTwist();
    // 6. BoundaryMatrix reduced_boundary_matrix =
    // reducer.reduce(boundary_matrix);
    // 7. auto representative_vectors =
    // Algorithms::ComputeRepresentativeVectors(reduced_boundary_matrix);
}
