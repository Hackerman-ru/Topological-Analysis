#include "complex.h"

#include "diagram.h"
#include "pairing.h"
#include "simplex.h"

#include <algorithm>

Complex::Complex(std::initializer_list<Simplex> simplices) : m_simplices(std::move(simplices)) {}

Complex::Complex(const std::vector<Simplex>& simplices) : m_simplices(simplices.begin(), simplices.end()) {}

bool Complex::is_valid() const {
    for (const auto& simplex : m_simplices) {
        for (const auto& subsimplex : simplex.subsimplices()) {
            if (!m_simplices.contains(subsimplex)) {
                return false;
            }
        }
    }
    return true;
}

std::optional<Complex> Complex::create(std::initializer_list<Simplex> simplices) {
    Complex result(std::move(simplices));
    if (result.is_valid()) {
        return result;
    } else {
        return std::nullopt;
    }
}

std::optional<Complex> Complex::create(const std::vector<Simplex>& simplices) {
    return Complex(simplices);
}

Complex Complex::create() {
    return Complex();
}

void Complex::push(const Simplex& simplex) {
    m_simplices.insert(simplex);
    for (auto subsimplex : simplex.subsimplices()) {
        push(subsimplex);
    }
}

std::vector<WeightedSimplex> Complex::weigh_simplices(Filter filter) const {
    std::vector<WeightedSimplex> weighted_simplices;
    weighted_simplices.reserve(m_simplices.size());
    for (const auto& simplex : m_simplices) {
        weighted_simplices.emplace_back(WeightedSimplex(simplex, filter));
    }
    sort(weighted_simplices.begin(), weighted_simplices.end());
    return weighted_simplices;
}

PersistenceDiagram Complex::generate_diagram(Filter filter) const {
    std::vector<WeightedSimplex> weighted_simplices = weigh_simplices(filter);
    PersistencePairing pp = create_pairing(weighted_simplices);
    return PersistenceDiagram(pp, filter);
}

std::string Complex::to_string() const {
    std::string result = "{";
    for (const auto& simplex : m_simplices) {
        result += simplex.to_string() + ",";
    }
    if (size() != 0) {
        result.pop_back();
    }
    result += '}';
    return result;
}

size_t Complex::size() const {
    return m_simplices.size();
}
