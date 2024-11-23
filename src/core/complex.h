#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include "diagram.h"
#include "simplex.h"

#include <functional>
#include <initializer_list>
#include <unordered_set>

class Complex {
public:
    using Weight = WeightedSimplex::Weight;
    using Filter = std::function<Weight(const Simplex&)>;

    static std::optional<Complex> create(std::initializer_list<Simplex> simplexes);
    static std::optional<Complex> create(const std::vector<Simplex>& simplices);
    static Complex create();

    void push(const Simplex& simplex);

    PersistenceDiagram generate_diagram(Filter filter) const;
    std::string to_string() const;
    size_t size() const;

private:
    Complex() = default;
    Complex(std::initializer_list<Simplex> simplexes);
    Complex(const std::vector<Simplex>& simplices);

    bool is_valid() const;
    std::vector<WeightedSimplex> weigh_simplices(Filter filter) const;

    std::unordered_set<Simplex> m_simplices;
};

#endif
