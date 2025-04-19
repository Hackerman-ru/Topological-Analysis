#include "reducer.hpp"
#include "persistence_diagram.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <format>
#include <string_view>

using namespace std::literals;
using namespace topa;

void PrintPersistenceDiagram(const PersistenceRepresentatives& reps) {
    // ANSI color codes
    constexpr auto BLUE = "\033[94m";
    constexpr auto CYAN = "\033[96m";
    constexpr auto GREEN = "\033[92m";
    constexpr auto YELLOW = "\033[93m";
    constexpr auto RESET = "\033[0m";

    constexpr int WIDTH = 70;
    std::cout << std::string(WIDTH, '-') << '\n'
              << BLUE << " Persistence Diagram (size: " << reps.size() << ")\n"
              << RESET << std::string(WIDTH, '-') << '\n';

    // Header
    std::cout << CYAN
              << "│  Birth  │  Death  │  Lifetime  │                        "
                 "Components (size)                     │\n"
              << "├─────────┼─────────┼────────────┼───────────────────────────"
                 "───────────────────────────────────┤\n"
              << RESET;

    for (const auto& rep : reps) {
        // Format output
        std::cout << "│ " << std::setw(7) << rep.birth << " "
                  << "│ " << std::setw(7) << rep.death << " "
                  << "│ " << GREEN << std::setw(6) << (rep.death - rep.birth)
                  << RESET << "     "
                  << "│ " << YELLOW << "Edges[B:" << std::setw(5)
                  << rep.birth_edges.size() << ", D:" << std::setw(5)
                  << rep.death_edges.size() << "] "
                  << "Vertices[B:" << std::setw(5) << rep.birth_vertices.size()
                  << ", D:" << std::setw(5) << rep.death_vertices.size() << "]"
                  << RESET << " │\n";
    }

    // Footer
    std::cout << std::string(WIDTH, '-') << '\n';
}

TEST_CASE("Harmonic Test") {
    std::cout << "1. Loading pointcloud...\n";
    auto pointcloud = Pointcloud::Load(DATA_DIR "/pointclouds/100.off");
    Filtration filtration = Filtration::FullVietorisRips();
    std::cout << "2. Filtering pointcloud...\n";
    FilteredComplex complex = FilteredComplex::From(*pointcloud, filtration);
    std::cout << "\tNumber of simplices: " << complex.GetSimplices().size()
              << '\n';
    std::cout << "3. Reducing boundary matrix...\n";
    Reducer reducer = Reducer::DoubleTwist();
    Lows lows = reducer.Reduce(complex);
    std::cout << "3. Computing persistence diagram with harmonic cycles...\n";
    auto begin = std::chrono::steady_clock::now();
    PersistenceRepresentatives reps = PersistenceDiagram(complex, lows);
    auto end = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(end - begin);
    std::cout << "Elapsed time for computing persistence diagram = "
              << elapsed.count() << "[s]" << '\n';
    std::cout << "4. Printing results...\n";
    PrintPersistenceDiagram(reps);
}
