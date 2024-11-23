#include "pairing.h"

#include "simplex.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>

class Table {
public:
    Table() = default;

    void push_weighted_simplex(const WeightedSimplex& weighted_simplex) {
        const Simplex& simplex = weighted_simplex.get_simplex();
        m_representatives[simplex] = {simplex};
        Column current_column(weighted_simplex);
        if (current_column.empty()) {
            m_pp[simplex] = {std::nullopt, m_representatives[simplex]};
            return;
        }
        size_t dimension = weighted_simplex.dimension();
        for (const auto& column : m_columns[dimension]) {
            if (column[0] > current_column[0]) {
                continue;
            }
            if (column[0] == current_column[0]) {
                current_column = current_column ^ column;
                const Simplex& column_header = column.header().get_simplex();
                for (const auto& representative : m_representatives[column_header]) {
                    m_representatives[simplex].emplace_back(representative);
                }
                if (!current_column.empty()) {
                    continue;
                }
            }
            break;
        }
        std::sort(m_representatives[simplex].begin(), m_representatives[simplex].end());
        if (current_column.empty()) {
            m_pp[simplex] = {std::nullopt, m_representatives[simplex]};
            return;
        }
        const Simplex& creator = current_column[0].get_simplex();
        m_pp[creator] = {simplex, m_representatives[creator]};
        m_columns[dimension].emplace_back(std::move(current_column));
        std::sort(m_columns[dimension].begin(),
                  m_columns[dimension].end(),
                  [](const Column& lhs, const Column& rhs) { return lhs[0] > rhs[0]; });
    }

    const PersistencePairing& get_pairing() const {
        return m_pp;
    }

private:
    class Column {
        using Elements = std::vector<WeightedSimplex>;

    public:
        Column(const WeightedSimplex& weighted_simplex) :
            m_header(weighted_simplex), m_elements(get_elements(weighted_simplex)) {};

        const WeightedSimplex& operator[](size_t pos) const {
            return m_elements[pos];
        }

        WeightedSimplex& operator[](size_t pos) {
            return m_elements[pos];
        }

        friend Column operator^(const Column& lhs, const Column& rhs) {
            assert(lhs[0] == rhs[0]);
            Elements elements;
            size_t lhs_pos = 1;
            size_t rhs_pos = 1;
            while (lhs_pos < lhs.size() || rhs_pos < rhs.size()) {
                if (lhs_pos == lhs.size()) {
                    elements.emplace_back(rhs[rhs_pos]);
                    ++rhs_pos;
                    continue;
                }
                if (rhs_pos == rhs.size()) {
                    elements.emplace_back(lhs[lhs_pos]);
                    ++lhs_pos;
                    continue;
                }
                if (lhs[lhs_pos] < rhs[rhs_pos]) {
                    elements.emplace_back(rhs[rhs_pos]);
                    ++rhs_pos;
                } else if (lhs[lhs_pos] > rhs[rhs_pos]) {
                    elements.emplace_back(lhs[lhs_pos]);
                    ++lhs_pos;
                } else {
                    ++lhs_pos;
                    ++rhs_pos;
                }
            }
            Column result = Column(lhs.m_header, std::move(elements));
            return result;
        }

        size_t size() const {
            return m_elements.size();
        }

        bool empty() const {
            return m_elements.empty();
        }

        const WeightedSimplex& header() const {
            return m_header;
        }

        const Elements& get_elements() const {
            return m_elements;
        }

    private:
        Column(const WeightedSimplex& header, Elements elements) :
            m_header(header), m_elements(std::move(elements)) {};

        Elements get_elements(const WeightedSimplex& weighted_simplex) {
            Elements result = weighted_simplex.subsimplices();
            std::reverse(result.begin(), result.end());
            return result;
        }

        WeightedSimplex m_header;
        Elements m_elements;
    };

    PersistencePairing m_pp;
    std::unordered_map<size_t, std::vector<Column>> m_columns;
    std::unordered_map<Simplex, Representatives> m_representatives;
};

PersistencePairing create_pairing(const std::vector<WeightedSimplex>& weighted_simplices) {
    Table table;
    for (const auto& weighted_simplex : weighted_simplices) {
        table.push_weighted_simplex(weighted_simplex);
    }
    return table.get_pairing();
}
