#ifndef TOPA_PERSISTENCE_PAIRING_HPP
#define TOPA_PERSISTENCE_PAIRING_HPP

#include "simplex.hpp"

#include <iostream>
#include <vector>

namespace topa {
    template<typename Vertex>
    class PersistencePairing {
        using simplex_t = Simplex<Vertex>;

    public:
        struct Pair {
            simplex_t creator;
            simplex_t destroyer;
        };

        using Pairs = std::vector<Pair>;

        PersistencePairing() = default;

        template<typename Container>
        PersistencePairing(Container&& pairs) : m_pairs(std::move(pairs)) {};

        void add_pair(simplex_t creator, simplex_t destroyer) {
            m_pairs.emplace_back(Pair {.creator = std::move(creator), .destroyer = std::move(destroyer)});
        }

        Pairs get_pairs() const {
            return m_pairs;
        }

        void show() const {
            std::cout << "==============================================\n";
            std::cout << "Persistence Pairing\n";
            std::cout << "----------------------------------------------\n";
            std::cout << "Pairs:\n";
            size_t number = 1;
            for (const auto& pair : m_pairs) {
                std::cout << number << ":\n";
                std::cout << "\tCreator: " << pair.creator.to_string() << '\n';
                std::cout << "\tDestroyer: " << pair.destroyer.to_string() << '\n';
                ++number;
            }
            std::cout << "==============================================\n";
        }

    private:
        Pairs m_pairs;
    };

    template<typename Vertex, typename Matrix, typename Reducer, typename Filtration,
             typename index_t = size_t>
    PersistencePairing<Vertex> compute_persistence_pairing(const Filtration& filtration) {
        PersistencePairing<Vertex> persistence_pairing;
        Matrix reduced_matrix = Reducer::reduce(filtration);
        auto simplices = filtration.simplices();
        for (index_t index = 0; index < reduced_matrix.size(); ++index) {
            if (!reduced_matrix[index].empty()) {
                Simplex creator = simplices[reduced_matrix[index].back()];
                Simplex destroyer = simplices[index];
                persistence_pairing.add_pair(std::move(creator), std::move(destroyer));
            }
        }
        return persistence_pairing;
    }
}   // namespace topa

#endif
