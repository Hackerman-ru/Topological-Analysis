#ifndef TOPA_COMPLEX_HPP
#define TOPA_COMPLEX_HPP

#include "simplex.hpp"

#include <iostream>
#include <unordered_map>

namespace topa {
    template<typename Vertex>
    class Complex {
        using simplex_t = Simplex<Vertex>;
        using Stats = std::unordered_map<size_t, size_t>;

    public:
        Complex() = default;

        template<typename InputIterator>
        Complex(InputIterator begin, InputIterator end) : m_simplices(begin, end) {
            for (const auto& simplex : m_simplices) {
                m_max_simplex_size = std::max(m_max_simplex_size, simplex.size());
            }
        }

        void add(simplex_t simplex) {
            if (m_simplices.contains(simplex)) {
                return;
            }

            ++stats[simplex.size()];

            if (simplex.size() > 1) {
                for (const auto& subsimplex : simplex.subsimplices()) {
                    add(subsimplex);
                }
            }

            m_max_simplex_size = std::max(m_max_simplex_size, simplex.size());
            m_simplices.insert(std::move(simplex));
        }

        void add_unchecked(simplex_t&& simplex) {
            ++stats[simplex.size()];
            m_max_simplex_size = std::max(m_max_simplex_size, simplex.size());
            m_simplices.insert(std::move(simplex));
        }

        bool is_valid() const {
            for (const auto& simplex : m_simplices) {
                for (const auto& subsimplex : simplex.subsimplices()) {
                    if (!m_simplices.contains(subsimplex)) {
                        return false;
                    }
                }
            }
            return true;
        }

        const std::set<simplex_t>& simplices() const {
            return m_simplices;
        }

        std::set<simplex_t> extract_simplices() {
            std::set<simplex_t> result = std::move(m_simplices);
            m_simplices.clear();
            m_max_simplex_size = 0;
            stats.clear();
            return result;
        }

        size_t size() const {
            return m_simplices.size();
        }

        size_t max_simplex_size() const {
            return m_max_simplex_size;
        }

        void show_stats() const {
            std::cout << "==============================================\n";
            std::cout << "Complex Stats\n";
            std::cout << "----------------------------------------------\n";
            std::cout << "Number of simplices:\n";
            size_t size = 1;
            while (stats.contains(size)) {
                std::cout << "Size: " << size << ". Number: " << stats.at(size) << "\n";
                ++size;
            }
            std::cout << "==============================================\n";
        }

    private:
        std::set<simplex_t> m_simplices;
        size_t m_max_simplex_size = 0;
        Stats stats;
    };
};   // namespace topa

#endif
