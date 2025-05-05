#pragma once

#include "common/type/harmonic_pair.hpp"

#include <iostream>
#include <iomanip>

namespace topa::detail {

void PrintCycles(HarmonicPairRange auto&& reps) {
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
              << "│  Birth  │  Death  │  Lifetime  │                   "
                 "Components (size)                │\n"
              << "├─────────┼─────────┼────────────┼───────────────────────────"
                 "─────────────────────────┤\n"
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

}  // namespace topa::detail
