#ifndef TOPA_FILTRATION_HPP
#define TOPA_FILTRATION_HPP

#include "complex.hpp"
#include "simplex.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <vector>

namespace topa {
    template<typename Vertex, typename Weight, typename index_t = size_t>
    class Filtration {
        using simplex_t = Simplex<Vertex>;
        using complex_t = Complex<Vertex>;
        using Filter = std::function<Weight(const simplex_t&)>;
        using Simplices = std::vector<simplex_t>;
        using Indexer = std::map<simplex_t, index_t>;
        using Indices = std::vector<index_t>;

    public:
        Filtration(complex_t complex, const Filter& filter) :
            m_simplices(sort_simplices(complex.extract_simplices(), filter)), m_filter(filter) {
            for (index_t index = 0; index < m_simplices.size(); ++index) {
                m_indexer[m_simplices[index]] = index;
                m_max_simplex_size = std::max(m_max_simplex_size, m_simplices[index].size());
            }
        }

        const Indexer& indexer() const {
            return m_indexer;
        }

        const Simplices& simplices() const {
            return m_simplices;
        }

        const Filter& filter() const {
            return m_filter;
        }

        size_t max_simplex_size() const {
            return m_max_simplex_size;
        }

    private:
        static Simplices sort_simplices(std::set<simplex_t> simplices, const Filter& filter) {
            Simplices sorted_simplices(std::move_iterator(simplices.begin()),
                                       std::move_iterator(simplices.end()));
            auto comparator = [&](const simplex_t& lhs, const simplex_t& rhs) {
                return filter(lhs) < filter(rhs);
            };
            std::sort(sorted_simplices.begin(), sorted_simplices.end(), comparator);
            return sorted_simplices;
        }

        Indexer m_indexer;
        Simplices m_simplices;
        Filter m_filter;
        size_t m_max_simplex_size = 0;
    };
};   // namespace topa

#endif
