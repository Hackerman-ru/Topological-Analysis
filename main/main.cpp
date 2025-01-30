#include "topa/topa.hpp"

#include <cassert>

int main() {
    using weight_t = float;
    static constexpr size_t dimensions = 3;
    using Vertex = topa::LightVertex<topa::Point<weight_t, dimensions>>;
    using Simplex = topa::Simplex<Vertex>;
    using Complex = topa::Complex<Vertex>;
    using index_t = size_t;
    using Column = std::vector<index_t>;
    using Matrix = std::vector<Column>;
    using Filtration = topa::Filtration<Vertex, weight_t>;
    using FastColumn = topa::BitTreeIndexColumn<index_t>;
    using Reducer = topa::DoubleTwistReducer<Matrix, FastColumn, Filtration>;
    using PersistentPairing = topa::PersistencePairing<Vertex>;

    std::cout << "Reading OFF file...\n";
    auto vertices = topa::read_pointcloud<Vertex, weight_t, dimensions>(DATA_DIR "/pointclouds/dragon.off");
    assert(vertices.has_value());
    auto distance = [](const Vertex& lhs, const Vertex& rhs) {
        weight_t result = 0;
        for (size_t i = 0; i < dimensions; ++i) {
            weight_t temp = (*lhs).at(i) - (*rhs).at(i);
            result += temp * temp;
        }
        return result;
    };
    std::cout << "Generating Vietoris-Rips complex...\n";
    Complex complex =
        topa::generate_vietoris_rips_complex<Vertex, weight_t>(vertices.value(), distance, 3, 0.006);
    complex.show_stats();
    auto filter = [&](const Simplex& simplex) {
        weight_t result = 0;
        if (simplex.size() <= 1) {
            return result;
        }
        auto simplex_vertices = simplex.vertices();
        for (auto it = simplex_vertices.begin(); it != simplex_vertices.end(); ++it) {
            auto jt = it;
            ++jt;
            for (; jt != simplex_vertices.end(); ++jt) {
                result = std::max(result, distance(*it, *jt));
            }
        }
        return result;
    };
    std::cout << "Building filtration...\n";
    Filtration filtration(std::move(complex), filter);
    std::cout << "Computing persistence pairing...\n";
    auto start = std::chrono::high_resolution_clock::now();
    PersistentPairing persistence_pairing =
        topa::compute_persistence_pairing<Vertex, Matrix, Reducer, Filtration, index_t>(
            std::move(filtration));
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "\e[1mTime taken by reduction: " << static_cast<double>(duration.count()) / 1000
              << " seconds\e[0m" << std::endl;
}
