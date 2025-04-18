#ifndef TOPA_PERSISTENCE_DIAGRAM_HPP_
#define TOPA_PERSISTENCE_DIAGRAM_HPP_

#include "filtered_complex.hpp"
#include "matrix.hpp"
#include "low.hpp"

namespace topa {

using HarmonicCycle = basic_types::DefaultContainer<float>;

struct PersistenceRepresentative {
    Position birth;
    Position death;
    int dim;
    HarmonicCycle birth_harmonic_cycle;
    HarmonicCycle death_harmonic_cycle;

    PersistenceRepresentative(Position b, Position d, int di)
        : birth(b),
          death(d),
          dim(di) {};

    bool operator<(const PersistenceRepresentative& other) const {
        if (dim != other.dim) {
            return dim < other.dim;
        }
        if (birth != other.birth) {
            return birth < other.birth;
        }
        return false;
    }
};

using PersistenceRepresentatives =
    basic_types::DefaultContainer<PersistenceRepresentative>;

PersistenceRepresentatives PersistenceDiagram(const FilteredComplex& complex,
                                              const Lows& lows);

};  // namespace topa

#endif
