#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include "simplex.h"

template<typename Vertex>
class Complex {
public:
    Complex() = default;

    Complex(std::initializer_list<Simplex<Vertex>> simplices) : m_simplices(std::move(simplices)) {
        std::sort(m_simplices.begin(), m_simplices.end());
    }

    Complex(std::vector<Simplex<Vertex>> simplices) : m_simplices(std::move(simplices)) {
        std::sort(m_simplices.begin(), m_simplices.end());
    }

    std::vector<WeightedSimplex<Vertex>> weigh_simplices(Filter<Vertex> filter) const {
        std::vector<WeightedSimplex<Vertex>> weighted_simplices;
        weighted_simplices.reserve(m_simplices.size());

        for (const Simplex<Vertex>& simplex : m_simplices) {
            weighted_simplices.emplace_back(WeightedSimplex(simplex, filter));
        }

        std::sort(weighted_simplices.begin(), weighted_simplices.end());
        return weighted_simplices;
    }

    std::string to_string() const {
        std::string result = "[";
        for (const auto& simplex : m_simplices) {
            result += simplex.to_string() + ",";
        }
        if (size() != 0) {
            result.pop_back();
        }
        result += ']';
        return result;
    }

    size_t size() const {
        return m_simplices.size();
    }

private:
    std::vector<Simplex<Vertex>> m_simplices;
};

#endif
