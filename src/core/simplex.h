#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include <algorithm>
#include <format>
#include <functional>
#include <initializer_list>
#include <inttypes.h>
#include <string>
#include <vector>

class Simplex {
public:
    using Vertex = uint32_t;

    constexpr Simplex(std::vector<Vertex> vertices) : m_vertices(std::move(vertices)) {
        std::sort(m_vertices.begin(), m_vertices.end());
    }

    template<typename T>
    constexpr Simplex(std::initializer_list<T> vertices) {
        m_vertices.reserve(vertices.size());
        for (const auto& vertex : vertices) {
            m_vertices.push_back(vertex);
        }
        std::sort(m_vertices.begin(), m_vertices.end());
    }

    friend bool operator==(const Simplex& lhs, const Simplex& rhs) = default;

    friend std::strong_ordering operator<=>(const Simplex& lhs, const Simplex& rhs) {
        if (lhs.dimension() != rhs.dimension()) {
            return lhs.dimension() <=> rhs.dimension();
        }
        return lhs.m_vertices <=> rhs.m_vertices;
    }

    std::string to_string() const;
    size_t dimension() const;
    std::vector<Simplex> subsimplices() const;
    const std::vector<Simplex::Vertex>& vertices() const;

private:
    friend class std::hash<Simplex>;

    std::vector<Vertex> m_vertices;
};

template<>
struct std::hash<Simplex> {
public:
    size_t operator()(const Simplex& simplex) const {
        size_t result = 0;
        for (const auto& vertex : simplex.m_vertices) {
            cantor(result, vertex);
        }
        return result;
    }

private:
    size_t cantor(size_t lhs, size_t rhs) const {
        return ((lhs + rhs + 1) * (lhs + rhs) >> 1) + rhs;
    }
};

class WeightedSimplex {
public:
    using Weight = size_t;
    using Filter = std::function<Weight(const Simplex&)>;

    WeightedSimplex(const Simplex& simplex, Filter filter);

    friend bool operator==(const WeightedSimplex& lhs, const WeightedSimplex& rhs) {
        return lhs.m_weight == rhs.m_weight && lhs.m_simplex == rhs.m_simplex;
    }

    friend std::strong_ordering operator<=>(const WeightedSimplex& lhs, const WeightedSimplex& rhs) {
        if (lhs.m_weight != rhs.m_weight) {
            return lhs.m_weight <=> rhs.m_weight;
        }
        return lhs.m_simplex <=> rhs.m_simplex;
    }

    const Simplex& get_simplex() const;
    Weight get_weight() const;
    std::string to_string() const;
    size_t dimension() const;
    std::vector<WeightedSimplex> subsimplices() const;

private:
    Simplex m_simplex;
    Weight m_weight;
    Filter m_filter;
};

#endif
