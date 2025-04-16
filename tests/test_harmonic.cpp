#include "reducer.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace topa;

TEST_CASE("Harmonic Test") {
    std::cout << "1. Loading pointcloud...\n";
    auto pointcloud = Pointcloud::Load(DATA_DIR "/pointclouds/fractal.off");
    Filtration filtration = Filtration::FullVietorisRips();
    std::cout << "2. Filtering pointcloud...\n";
    Complex complex = Complex::From(*pointcloud, filtration);
    std::cout << "\tNumber of simplices: " << complex.GetSimplices().size()
              << '\n';
    std::cout << "3. Reducing boundary matrix...\n";
    std::cout << "3.1 Twist:\n";
    Reducer reducer = Reducer::Twist();
    Matrix<size_t> twist_reduced_boundary_matrix = reducer.Reduce(complex);
    std::cout << "3.2 Double Twist:\n";
    reducer = Reducer::DoubleTwist();
    Matrix<size_t> double_twist_reduced_boundary_matrix =
        reducer.Reduce(complex);
    // 7. auto representative_vectors =
    // Algorithms::ComputeRepresentativeVectors(reduced_boundary_matrix);
}
