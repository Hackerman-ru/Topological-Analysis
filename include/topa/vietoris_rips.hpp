#ifndef TOPA_VIETORIS_RIPS_HPP
#define TOPA_VIETORIS_RIPS_HPP

#include "complex.hpp"
#include "simplex.hpp"

#include <functional>
#include <iostream>
#include <optional>

namespace topa {
    // TODO: need to refactor this spagetti
    template<typename Vertex, typename distance_t>
    Complex<Vertex> generate_vietoris_rips_complex(
        const std::vector<Vertex>& vertices,
        const std::function<distance_t(const Vertex&, const Vertex&)>& distance,
        std::optional<size_t> max_dimension = std::nullopt,
        std::optional<distance_t> max_distance = std::nullopt) {
        using Simplex = Simplex<Vertex>;
        using Complex = Complex<Vertex>;

        Complex complex;
        size_t current_dimension = 0;
        std::vector<std::vector<Vertex>> previous_simplices;
        while (current_dimension < vertices.size()
               && (!max_dimension.has_value() || current_dimension < max_dimension.value())) {
            ++current_dimension;
            std::vector<std::vector<Vertex>> current_simplices;
            for (size_t i = 0; i < vertices.size(); ++i) {
                if (current_dimension == 1) {
                    current_simplices.push_back({vertices[i]});
                    auto simplices = current_simplices.back();
                    complex.add_unchecked(Simplex(simplices.begin(), simplices.end()));
                    continue;
                }
                for (size_t j = 0; j < previous_simplices.size(); ++j) {
                    bool fit = true;
                    for (size_t k = 0; k < previous_simplices[j].size(); ++k) {
                        if (previous_simplices[j][k] == vertices[i]) {
                            fit = false;
                            break;
                        }
                        if (max_distance.has_value()
                            && distance(vertices[i], previous_simplices[j][k]) > max_distance.value()) {
                            fit = false;
                            break;
                        }
                    }
                    if (fit) {
                        current_simplices.emplace_back(previous_simplices[j]);
                        current_simplices.back().emplace_back(vertices[i]);
                        auto simplices = current_simplices.back();
                        complex.add_unchecked(Simplex(simplices.begin(), simplices.end()));
                    }
                }
            }
            if (current_simplices.size() == 0) {
                break;
            }
            previous_simplices = std::move(current_simplices);
        }

        std::cout << "Number of simplices: " << complex.size() << '\n';
        return complex;
    }
}   // namespace topa

#endif
