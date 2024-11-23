#include "../src/include/matrix_algo.h"

#include "gtest/gtest.h"
#include <complex.h>

void dump_diagram(const PersistenceDiagram& pd) {
    std::cout << "Persistence pairing:\n";
    for (const auto& entry : pd.get_entries()) {
        std::string exterminator = entry.exterminator.has_value() ? entry.exterminator->to_string() : "none";
        std::string representatives = "{";
        for (const auto& representative : entry.representatives) {
            representatives += representative.to_string();
        }
        representatives += '}';
        std::cout << std::format("({}, {}) {}\n", entry.creator.to_string(), exterminator, representatives);
    }
    std::cout << "Persistence diagram:\n";
    for (const auto& entry : pd.get_weight_entries()) {
        std::string exterminator =
            entry.exterminator.has_value() ? std::to_string(entry.exterminator.value()) : "none";
        std::cout << std::format("({}, {}) {}\n", entry.creator, exterminator, entry.value);
    }
}

using Weight = WeightedSimplex::Weight;
using Vertex = Simplex::Vertex;

TEST(Diagram, Computation1) {
    auto filter = [](const Simplex& simplex) {
        static std::unordered_map<Simplex, Weight> mp = {
            {      Simplex {1}, 10},
            {      Simplex {2}, 40},
            {      Simplex {3}, 30},
            {      Simplex {4}, 20},
            {   Simplex {1, 2}, 80},
            {   Simplex {1, 4}, 20},
            {   Simplex {2, 3}, 60},
            {   Simplex {2, 4}, 50},
            {   Simplex {3, 4}, 50},
            {Simplex {2, 3, 4}, 70},
        };
        return mp[simplex];
    };

    auto complex = Complex::create({
        {1},
        {2},
        {3},
        {4},
        {1, 2},
        {1, 4},
        {2, 3},
        {2, 4},
        {3, 4},
        {2, 3, 4}
    });
    EXPECT_TRUE(complex.has_value());
    auto diagram = complex->generate_diagram(filter);
    dump_diagram(diagram);
}

Weight dist(const Vertex& lhs, const Vertex& rhs) {
    if (lhs >= rhs) {
        return lhs - rhs;
    } else {
        return rhs - lhs;
    }
}

Weight weight(const Simplex& simplex) {
    Weight w = 0;
    auto vertices = simplex.vertices();
    for (const auto& lhs_vertex : vertices) {
        for (const auto& rhs_vertex : vertices) {
            if (lhs_vertex == rhs_vertex) {
                continue;
            }
            w = std::max(w, dist(lhs_vertex, rhs_vertex));
        }
    }
    return w;
}

std::vector<Simplex> full_simplices(size_t n) {
    std::vector<Vertex> vertices;
    vertices.reserve(n);
    for (size_t i = 1; i <= n; ++i) {
        vertices.push_back(i);
    }
    std::vector<Simplex> result;
    for (const auto& first : vertices) {
        for (const auto& second : vertices) {
            if (second == first) {
                continue;
            }
            for (const auto& third : vertices) {
                if (third == second || third == first) {
                    continue;
                }
                result.emplace_back(Simplex {first, second, third});
            }
        }
    }
    return result;
}

TEST(Diagram, Computation2) {
    auto filter = [](const Simplex& simplex) -> Weight {
        if (simplex.dimension() == 1) {
            return 0;
        } else if (simplex.dimension() == 2) {
            auto vertices = simplex.vertices();
            return dist(vertices[0], vertices[1]);
        } else {
            return weight(simplex);
        }
    };
    size_t n = 30;
    auto complex = Complex::create(full_simplices(n));
    EXPECT_TRUE(complex.has_value());
    auto start = std::chrono::system_clock::now();
    auto diagram = complex->generate_diagram(filter);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    dump_diagram(diagram);
    std::cout << "diagram generation elapsed time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_seconds).count() << "ms\n";
}
