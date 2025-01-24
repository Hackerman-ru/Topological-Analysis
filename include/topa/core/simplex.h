#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include "util.h"

#include <algorithm>
#include <compare>
#include <functional>
#include <string>
#include <vector>

template<typename Vertex>
class Simplex {
public:
    Simplex(std::vector<Vertex> vertices) : m_vertices(std::move(vertices)) {
        find_subsimplices();
    }

    Simplex(std::initializer_list<Vertex> vertices) : m_vertices(std::move(vertices)) {
        find_subsimplices();
    }

    friend bool operator==(const Simplex& lhs, const Simplex& rhs) = default;

    friend std::partial_ordering operator<=>(const Simplex& lhs, const Simplex& rhs) {
        if (lhs.size() != rhs.size()) {
            return lhs.size() <=> rhs.size();
        }
        return lhs.m_vertices <=> rhs.m_vertices;
    }

    std::string to_string() const {
        std::string result = "{";
        for (const auto& vertex : m_vertices) {
            result += vertex.to_string() + ",";
        }
        if (size() > 0) {
            result.pop_back();
        }
        result += '}';
        return result;
    }

    size_t size() const {
        return m_vertices.size();
    }

    const std::vector<Simplex>& subsimplices() const {
        return m_subsimplices;
    }

    const std::vector<Vertex>& vertices() const {
        return m_vertices;
    }

private:
    void find_subsimplices() {
        if (m_vertices.size() <= 1) {
            return;
        }

        m_subsimplices.reserve(m_vertices.size());
        for (size_t exclude_pos = 0; exclude_pos < m_vertices.size(); ++exclude_pos) {
            std::vector<Vertex> subsimplex_vertices;
            subsimplex_vertices.reserve(m_vertices.size() - 1);
            for (size_t i = 0; i < m_vertices.size(); ++i) {
                if (i != exclude_pos) {
                    subsimplex_vertices.emplace_back(m_vertices[i]);
                }
            }
            m_subsimplices.push_back(std::move(subsimplex_vertices));
        }
    }

    std::vector<Vertex> m_vertices;
    std::vector<Simplex> m_subsimplices;
};

namespace std {
    template<typename Vertex>
    struct hash<Simplex<Vertex>> {
        auto operator()(const Simplex<Vertex>& simplex) const -> std::size_t {
            auto vertices = simplex.vertices();
            std::string ids;
            for (const auto& vertex : vertices) {
                ids += std::to_string(vertex.get_id());
            }
            return hash<std::string> {}(ids);
        }
    };
}   // namespace std

template<typename Vertex>
using Filter = std::function<Weight(const Simplex<Vertex>&)>;

template<typename Vertex>
class WeightedSimplex {
public:
    WeightedSimplex(const Simplex<Vertex>& simplex, Filter<Vertex> filter) :
        m_simplex(simplex), m_weight(filter(simplex)), m_filter(filter) {};

    friend std::partial_ordering operator<=>(const WeightedSimplex& lhs, const WeightedSimplex& rhs) {
        if (lhs.m_weight != rhs.m_weight) {
            return lhs.m_weight <=> rhs.m_weight;
        }
        return lhs.m_simplex <=> rhs.m_simplex;
    }

    const Simplex<Vertex>& get_simplex() const {
        return m_simplex;
    }

    Weight get_weight() const {
        return m_weight;
    }

    std::string to_string() const {
        return m_simplex.to_string();
    }

    size_t size() const {
        return m_simplex.size();
    }

    std::vector<WeightedSimplex> subsimplices() const {
        const auto& simplex_subsimplices = m_simplex.subsimplices();
        std::vector<WeightedSimplex> weighted_subsimplices;
        weighted_subsimplices.reserve(simplex_subsimplices.size());

        for (const auto& subsimplex : simplex_subsimplices) {
            weighted_subsimplices.emplace_back(WeightedSimplex(subsimplex, m_filter));
        }

        std::sort(weighted_subsimplices.begin(), weighted_subsimplices.end());
        return weighted_subsimplices;
    }

private:
    Simplex<Vertex> m_simplex;
    Weight m_weight;
    Filter<Vertex> m_filter;
};

#endif
