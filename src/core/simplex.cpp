#include "simplex.h"

std::string Simplex::to_string() const {
    std::string result = "[";
    for (const auto& vertex : m_vertices) {
        result += std::format("{},", std::to_string(vertex));
    }
    if (dimension() > 0) {
        result.pop_back();
    }
    result += ']';
    return result;
}

size_t Simplex::dimension() const {
    return m_vertices.size();
}

std::vector<Simplex> Simplex::subsimplices() const {
    std::vector<Simplex> result;
    result.reserve(m_vertices.size());
    for (size_t exclude_pos = 0; exclude_pos < m_vertices.size(); ++exclude_pos) {
        std::vector<Vertex> simplex;
        for (size_t i = 0; i < m_vertices.size(); ++i) {
            if (i != exclude_pos) {
                simplex.emplace_back(m_vertices[i]);
            }
        }
        if (!simplex.empty()) {
            result.push_back(std::move(simplex));
        }
    }
    return result;
}

const std::vector<Simplex::Vertex>& Simplex::vertices() const {
    return m_vertices;
}

WeightedSimplex::WeightedSimplex(const Simplex& simplex, Filter filter) :
    m_simplex(simplex), m_weight(filter(simplex)), m_filter(filter) {};

const Simplex& WeightedSimplex::get_simplex() const {
    return m_simplex;
}

WeightedSimplex::Weight WeightedSimplex::get_weight() const {
    return m_weight;
}

std::string WeightedSimplex::to_string() const {
    return m_simplex.to_string();
}

size_t WeightedSimplex::dimension() const {
    return m_simplex.dimension();
}

std::vector<WeightedSimplex> WeightedSimplex::subsimplices() const {
    const auto& simplex_subsimplices = m_simplex.subsimplices();
    std::vector<WeightedSimplex> result;
    result.reserve(simplex_subsimplices.size());
    for (const auto& subsimplex : simplex_subsimplices) {
        result.emplace_back(WeightedSimplex(subsimplex, m_filter));
    }
    std::sort(result.begin(), result.end());
    return result;
}
