#ifndef __INDEX_MATRIX_H__
#define __INDEX_MATRIX_H__

#include "simplex.h"

#include <cstdint>
#include <map>

using Index = size_t;
// using Value = bool;

static constexpr Index NO_INDEX = SIZE_MAX;

struct IndexElement {
    Index index;

    // Value value;

    friend bool operator==(const IndexElement& lhs, const IndexElement& rhs) {
        return lhs.index == rhs.index;
    }

    friend std::strong_ordering operator<=>(const IndexElement& lhs, const IndexElement& rhs) {
        return lhs.index <=> rhs.index;
    }
};

class IndexColumn {
public:
    IndexColumn(Index index);
    IndexColumn(Index index, std::vector<IndexElement> elements);
    IndexColumn(Index index, std::vector<IndexElement> elements, std::vector<Index> representatives);

    friend IndexColumn operator+(const IndexColumn& lhs, const IndexColumn& rhs);

    void clear();

    Index index() const;
    Index lowest_element_index() const;
    const std::vector<Index>& representatives() const;
    size_t size() const;

private:
    Index m_index;
    std::vector<IndexElement> m_elements;
    std::vector<Index> m_representatives;
};

class IndexMatrix {
public:
    IndexMatrix(Index size);

    template<typename Vertex>
    IndexMatrix(const std::vector<WeightedSimplex<Vertex>>& weighted_simplices) :
        m_columns(extract_columns<Vertex>(weighted_simplices)) {};

    const std::vector<IndexColumn>& get_columns() const;

    const IndexColumn& operator[](Index index) const;
    IndexColumn& operator[](Index index);
    size_t size() const;

private:
    static std::vector<IndexColumn> generate_empty_columns(Index size);

    template<typename Vertex>
    static std::vector<IndexColumn>
        extract_columns(const std::vector<WeightedSimplex<Vertex>>& weighted_simplices) {
        std::vector<IndexColumn> columns;
        columns.reserve(weighted_simplices.size());
        std::map<WeightedSimplex<Vertex>, Index> indexer;

        for (size_t index = 0; index < weighted_simplices.size(); ++index) {
            const WeightedSimplex<Vertex>& weighted_simplex = weighted_simplices[index];
            std::vector<IndexElement> elements = extract_elements<Vertex>(weighted_simplex, indexer);
            indexer[weighted_simplex] = index;
            columns.emplace_back(IndexColumn(index, std::move(elements)));
        }

        return columns;
    }

    template<typename Vertex>
    static std::vector<IndexElement>
        extract_elements(const WeightedSimplex<Vertex>& weighted_simplex,
                         const std::map<WeightedSimplex<Vertex>, Index>& indexer) {
        std::vector<IndexElement> elements;

        for (const WeightedSimplex<Vertex>& weighted_subsimplex : weighted_simplex.subsimplices()) {
            Index index = indexer.at(weighted_subsimplex);
            elements.emplace_back(IndexElement {.index = index});
        }

        return elements;
    }

    std::vector<IndexColumn> m_columns;
};

#endif
