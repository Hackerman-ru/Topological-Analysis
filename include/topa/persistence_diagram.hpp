#ifndef TOPA_PERSISTENCE_DIAGRAM_HPP_
#define TOPA_PERSISTENCE_DIAGRAM_HPP_

#include "filtered_complex.hpp"
#include "matrix.hpp"
#include "low.hpp"

namespace topa {

using HarmonicCycle = basic_types::DefaultContainer<float>;

struct PersistenceRepresentative {
    Position birth;
    HarmonicCycle birth_edges;
    HarmonicCycle birth_vertices;
    Position death;
    HarmonicCycle death_edges;
    HarmonicCycle death_vertices;

    PersistenceRepresentative(Position b, Position d)
        : birth(b),
          death(d) {};
};

using PersistenceRepresentatives =
    basic_types::DefaultContainer<PersistenceRepresentative>;

PersistenceRepresentatives PersistenceDiagram(const FilteredComplex& complex,
                                              const Lows& lows);

};  // namespace topa

#endif
