#include "core/complex.h"
#include "fast_generator.h"
#include "off_parser.h"
#include "vertices/point.h"

#include "gtest/gtest.h"

TEST(Pairing, TestBase) {
    using Vertex = Base;
    using Simplex = Simplex<Vertex>;
    using Complex = Complex<Vertex>;

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
    using Vertex = Point<int64_t, 2>;
    using Simplex = Simplex<Vertex>;
    using Complex = Complex<Vertex>;

    auto filter = [](const Simplex& simplex) -> Weight {
        auto vertices = simplex.vertices();
        if (simplex.size() <= 1) {
            return 0;
        }
        if (simplex.size() == 2) {
            return squared_distance(vertices[0], vertices[1]);
        }
        Weight max_weight = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            for (size_t j = i + 1; j < vertices.size(); ++j) {
                max_weight = std::max(max_weight, squared_distance(vertices[i], vertices[j]));
            }
        }
        return max_weight;
    };

    auto complex = Complex::create({
        {{Vertex(1, {1, 0})}},
        {{Vertex(2, {1, 2})}},
        {{Vertex(3, {-1, -2})}},
        {{Vertex(1, {1, 0}), Vertex(2, {1, 2})}},
        {{Vertex(1, {1, 0}), Vertex(3, {-1, -2})}},
        {{Vertex(2, {1, 2}), Vertex(3, {-1, -2})}},
        {{Vertex(1, {1, 0}), Vertex(2, {1, 2}), Vertex(3, {-1, -2})}},
    });

    EXPECT_TRUE(complex.has_value());
    std::cout << "Complex:\n";
    std::cout << complex->to_string() << '\n';
    auto weighted_simplices = complex->weigh_simplices(filter);
    auto persistence_pairing = generate_persistence_pairing<Vertex>(weighted_simplices);
    persistence_pairing.show();
}

TEST(Pairing, TestOFFParser) {
    using data_t = double;
    constexpr size_t dimensions = 3;
    using Vertex = Point<data_t, dimensions>;
    using Simplex = Simplex<Vertex>;

    auto filter = [](const Simplex& simplex) -> Weight {
        auto vertices = simplex.vertices();
        if (simplex.size() <= 1) {
            return 0;
        }
        if (simplex.size() == 2) {
            return squared_distance(vertices[0], vertices[1]);
        }
        Weight max_weight = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            for (size_t j = i + 1; j < vertices.size(); ++j) {
                max_weight = std::max(max_weight, squared_distance(vertices[i], vertices[j]));
            }
        }
        return max_weight;
    };

    auto complex = parse_off_file<data_t, dimensions>(DATA_DIR "/ModelNet40/guitar.off");
    EXPECT_TRUE(complex.has_value());
    auto weighted_simplices = complex->weigh_simplices(filter);
    auto persistence_pairing = generate_persistence_pairing<Vertex>(weighted_simplices);
    //persistence_pairing.show();
}
