#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include "simplex.h"

#include <optional>
#include <set>

template<typename Vertex>
class Complex {
public:
    static std::optional<Complex> create(std::initializer_list<Simplex<Vertex>> simplices) {
        Complex complex(std::move(simplices));
        if (complex.is_valid()) {
            return complex;
        } else {
            return std::nullopt;
        }
    }

    static std::optional<Complex> create(std::set<Simplex<Vertex>> simplices) {
        Complex complex(std::move(simplices));
        if (complex.is_valid()) {
            return complex;
        } else {
            return std::nullopt;
        }
    }

    template<typename Iterator>
    static std::optional<Complex> create(Iterator begin, Iterator end) {
        std::set<Simplex<Vertex>> simplices;
        while (begin != end) {
            simplices.insert(*begin);
            ++begin;
        }
        Complex complex(std::move(simplices));
        if (complex.is_valid()) {
            return complex;
        } else {
            return std::nullopt;
        }
    }

    static Complex create() {
        return Complex();
    }

    void push(Simplex<Vertex> simplex) {
        if (m_simplices.contains(simplex)) {
            return;
        }

        for (const Simplex<Vertex>& subsimplex : simplex.subsimplices()) {
            push(subsimplex);
        }

        m_simplices.insert(std::move(simplex));
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
    Complex() = default;

    Complex(std::initializer_list<Simplex<Vertex>> simplices) : m_simplices(std::move(simplices)) {}

    Complex(std::set<Simplex<Vertex>> simplices) : m_simplices(std::move(simplices)) {}

    bool is_valid() const {
        for (const Simplex<Vertex>& simplex : m_simplices) {
            for (const Simplex<Vertex>& subsimplex : simplex.subsimplices()) {
                if (!m_simplices.contains(subsimplex)) {
                    return false;
                }
            }
        }
        return true;
    }

    std::set<Simplex<Vertex>> m_simplices;
};

#endif
