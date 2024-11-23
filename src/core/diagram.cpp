#include "diagram.h"

PersistenceDiagram::PersistenceDiagram(PersistencePairing pp, Filter filter) {
    for (const auto& [creator, data] : pp) {
        const auto& [exterminarot, representatives] = data;
        m_entries.emplace_back(
            Entry {.creator = creator, .exterminator = exterminarot, .representatives = representatives});
        std::optional<Weight> exterminator_weight = std::nullopt;
        if (exterminarot.has_value()) {
            exterminator_weight = filter(exterminarot.value());
        }
        m_weight_entries.emplace_back(WeightEntry {.creator = filter(creator),
                                                   .exterminator = exterminator_weight,
                                                   .value = representatives.size() > 1});
    }
    std::sort(m_entries.begin(), m_entries.end(), [](const Entry& lhs, const Entry& rhs) {
        if (lhs.exterminator.has_value() != rhs.exterminator.has_value()) {
            return lhs.exterminator.has_value();
        }
        return lhs.creator < rhs.creator;
    });
    std::sort(m_weight_entries.begin(),
              m_weight_entries.end(),
              [](const WeightEntry& lhs, const WeightEntry& rhs) { return lhs.creator < rhs.creator; });
}

std::vector<PersistenceDiagram::Entry> PersistenceDiagram::get_entries() const {
    return m_entries;
}

std::vector<PersistenceDiagram::WeightEntry> PersistenceDiagram::get_weight_entries() const {
    return m_weight_entries;
}
