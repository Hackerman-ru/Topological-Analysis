#ifndef __PAIRING_H__
#define __PAIRING_H__

#include "complex.h"

#include <iostream>

template<typename Vertex>
struct PersistencePair {
    WeightedSimplex<Vertex> creator;
    WeightedSimplex<Vertex> destroyer;
    std::vector<Simplex<Vertex>> representatives;
};

template<typename Vertex>
struct Essential {
    WeightedSimplex<Vertex> weighted_simplex;
    std::vector<Simplex<Vertex>> representatives;
};

template<typename Vertex>
class PersistencePairing {
    using Pairs = std::vector<PersistencePair<Vertex>>;
    using Essentials = std::vector<Essential<Vertex>>;

public:
    PersistencePairing() = default;

    void add_pair(PersistencePair<Vertex> pair) {
        m_pairs.emplace_back(std::move(pair));
    }

    void add_essential(Essential<Vertex> essential) {
        m_essentials.emplace_back(std::move(essential));
    }

    Pairs pairs() const {
        return m_pairs;
    }

    Essentials essentials() const {
        return m_essentials;
    }

    void show() const {
        std::cout << "==============================================\n";
        std::cout << "Persistence Pairing\n";
        std::cout << "----------------------------------------------\n";
        std::cout << "Pairs:\n";
        size_t number = 1;
        for (const PersistencePair<Vertex>& pair : m_pairs) {
            std::cout << number << ":\n";
            std::cout << "  Creator: " << pair.creator.to_string() << '\n';
            std::cout << "  Destroyer: " << pair.destroyer.to_string() << '\n';
            show_representatives(pair.representatives);
            ++number;
        }
        std::cout << "----------------------------------------------\n";
        std::cout << "Essentials:\n";
        number = 1;
        for (const Essential<Vertex>& essential : m_essentials) {
            std::cout << number << ":\n";
            std::cout << "  Essential: " << essential.weighted_simplex.to_string() << '\n';
            show_representatives(essential.representatives);
            ++number;
        }
        std::cout << std::right;
        std::cout << "==============================================\n";
    }

private:
    static void show_representatives(const std::vector<Simplex<Vertex>>& representatives) {
        std::cout << "  Representatives: ";
        std::cout << '[';
        for (size_t i = 0; i < representatives.size(); ++i) {
            std::cout << representatives[i].to_string();
            if (i != representatives.size() - 1) {
                std::cout << " + ";
            }
        }
        std::cout << "]\n";
    }

    Pairs m_pairs;
    Essentials m_essentials;
};

#endif
