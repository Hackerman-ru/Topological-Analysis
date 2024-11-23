#ifndef __DIAGRAM_H__
#define __DIAGRAM_H__

#include "pairing.h"
#include "simplex.h"

#include <optional>

class PersistenceDiagram {
    using Weight = WeightedSimplex::Weight;
    using Filter = WeightedSimplex::Filter;

public:
    struct Entry {
        Simplex creator;
        std::optional<Simplex> exterminator;
        Representatives representatives;
    };

    struct WeightEntry {
        Weight creator;
        std::optional<Weight> exterminator;
        size_t value;
    };

    PersistenceDiagram(PersistencePairing pp, Filter filter);

    std::vector<Entry> get_entries() const;
    std::vector<WeightEntry> get_weight_entries() const;

private:
    std::vector<Entry> m_entries;
    std::vector<WeightEntry> m_weight_entries;
};
#endif
