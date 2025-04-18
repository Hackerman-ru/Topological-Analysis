#include "reducer.hpp"
#include "persistence_diagram.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace topa;

void PrintPersistenceDiagram(const PersistenceRepresentatives& reps) {
    // ANSI color codes
    constexpr auto BLUE = "\033[94m";
    constexpr auto CYAN = "\033[96m";
    constexpr auto GREEN = "\033[92m";
    constexpr auto YELLOW = "\033[93m";
    constexpr auto RESET = "\033[0m";

    constexpr int WIDTH = 80;
    std::cout << std::string(WIDTH, '-') << '\n'
              << BLUE << " Persistence Diagram (size: " << reps.size() << ")\n"
              << RESET << std::string(WIDTH, '-') << '\n';

    // Header
    std::cout << CYAN
              << "│ Dim │ Birth │ Death │  LifeTime  │   Cycles (size)   │\n"
              << "├─────┼───────┼───────┼────────────┼───────────────────┤\n"
              << RESET;

    for (const auto& rep : reps) {
        // Format cycles info
        if (rep.dim != 1) {
            continue;
        }
        std::stringstream cycles;
        cycles << "[B:";
        if (!rep.birth_harmonic_cycle.empty()) {
            cycles << rep.birth_harmonic_cycle.size() << "] [D:";
        } else {
            cycles << "N/A] [D:";
        }
        if (!rep.death_harmonic_cycle.empty()) {
            cycles << rep.death_harmonic_cycle.size() << "]";
        } else {
            cycles << "N/A]";
        }

        // Format output
        std::cout << "│ " << std::setw(2) << rep.dim << " "
                  << "│ " << std::setw(4) << rep.birth << " "
                  << "│ " << std::setw(4) << rep.death << " "
                  << "│ " << GREEN << std::setw(6) << (rep.death - rep.birth)
                  << RESET << "    "
                  << "│ " << YELLOW << std::setw(17) << std::left
                  << cycles.str() << RESET << std::right << " │\n";
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
