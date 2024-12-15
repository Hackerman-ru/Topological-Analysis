#include "../src/include/reducers.h"
#include "../src/include/vertices.h"

#include "gtest/gtest.h"

TEST(Pairing, TestBase) {
    using Vertex = Base;
    using Simplex = Simplex<Vertex>;
    using Complex = Complex<Vertex>;
    using PersistencePairing = PersistencePairing<Vertex>;

    auto filter = [](const Simplex& simplex) {
        static std::map<Simplex, Weight> mp = {
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
    std::cout << "Complex:\n";
    std::cout << complex->to_string() << '\n';
    auto weighted_simplices = complex->weigh_simplices(filter);
    auto persistence_pairing = generate_persistence_pairing<Vertex>(weighted_simplices);
    persistence_pairing.show();
}

TEST(Pairing, TestPoint) {
    using Vertex = Point;
    using Simplex = Simplex<Vertex>;
    using Complex = Complex<Vertex>;
    using PersistencePairing = PersistencePairing<Vertex>;

    auto filter = [](const Simplex& simplex) -> Weight {
        auto vertices = simplex.vertices();
        if (simplex.size() <= 1) {
            return 0;
        } else if (simplex.size() == 2) {
            return squared_distance(vertices[0], vertices[1]);
        } else {
            Weight max_weight = 0;
            for (size_t i = 0; i < vertices.size(); ++i) {
                for (size_t j = i + 1; j < vertices.size(); ++j) {
                    max_weight = std::max(max_weight, squared_distance(vertices[i], vertices[j]));
                }
            }
            return max_weight;
        }
    };

    auto s = Simplex({
        Point {1, 1, 0}
    });

    auto complex = Complex::create({
        {{{1, 1, 0}}},
        {{{2, 1, 2}}},
        {{{3, -1, -2}}},
        {{{1, 1, 0}, {2, 1, 2}}},
        {{{1, 1, 0}, {3, -1, -2}}},
        {{{2, 1, 2}, {3, -1, -2}}},
        {{{1, 1, 0}, {2, 1, 2}, {3, -1, -2}}},
    });
    EXPECT_TRUE(complex.has_value());
    std::cout << "Complex:\n";
    std::cout << complex->to_string() << '\n';
    auto weighted_simplices = complex->weigh_simplices(filter);
    auto persistence_pairing = generate_persistence_pairing<Vertex>(weighted_simplices);
    persistence_pairing.show();
}
