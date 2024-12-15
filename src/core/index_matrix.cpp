#include "index_matrix.h"

IndexColumn::IndexColumn(Index index) : m_index(index), m_representatives({index}) {};

IndexColumn::IndexColumn(Index index, std::vector<IndexElement> elements) :
    m_index(index), m_elements(std::move(elements)), m_representatives({index}) {};

IndexColumn::IndexColumn(Index index, std::vector<IndexElement> elements,
                         std::vector<Index> representatives) :
    m_index(index), m_elements(std::move(elements)), m_representatives(std::move(representatives)) {};

IndexColumn operator+(const IndexColumn& lhs, const IndexColumn& rhs) {
    std::vector<IndexElement> elements;
    size_t lhs_pos = 0;
    size_t rhs_pos = 0;

    while (lhs_pos < lhs.size() || rhs_pos < rhs.size()) {
        if (lhs_pos == lhs.size()) {
            elements.emplace_back(rhs.m_elements[rhs_pos]);
            ++rhs_pos;
            continue;
        }
        if (rhs_pos == rhs.size()) {
            elements.emplace_back(lhs.m_elements[lhs_pos]);
            ++lhs_pos;
            continue;
        }
        if (lhs.m_elements[lhs_pos] < rhs.m_elements[rhs_pos]) {
            elements.emplace_back(lhs.m_elements[lhs_pos]);
            ++lhs_pos;
        } else if (lhs.m_elements[lhs_pos] > rhs.m_elements[rhs_pos]) {
            elements.emplace_back(rhs.m_elements[rhs_pos]);
            ++rhs_pos;
        } else {
            // VALUE ADDITION LOGIC?
            ++lhs_pos;
            ++rhs_pos;
        }
    }

    std::vector<Index> representatives = rhs.m_representatives;
    representatives.emplace_back(lhs.index());
    return IndexColumn {rhs.index(), std::move(elements), std::move(representatives)};
}

void IndexColumn::clear_and_set_representatives(std::vector<Index> representatives) {
    m_elements.clear();
    m_representatives = std::move(representatives);
}

Index IndexColumn::index() const {
    return m_index;
}

Index IndexColumn::lowest_element_index() const {
    if (m_elements.empty()) {
        return NO_INDEX;
    }
    return m_elements.back().index;
}

const std::vector<IndexElement>& IndexColumn::elements() const {
    return m_elements;
}

const std::vector<Index>& IndexColumn::representatives() const {
    return m_representatives;
}

size_t IndexColumn::size() const {
    return m_elements.size();
}

IndexMatrix::IndexMatrix(Index size) : m_columns(generate_empty_columns(size)) {};

const std::vector<IndexColumn>& IndexMatrix::get_columns() const {
    return m_columns;
}

const std::vector<Index>& IndexMatrix::subindices(Index index) const {
    return m_subindices.at(index);
}

const IndexColumn& IndexMatrix::operator[](Index index) const {
    return m_columns[index];
}

IndexColumn& IndexMatrix::operator[](Index index) {
    return m_columns[index];
}

size_t IndexMatrix::size() const {
    return m_columns.size();
}

std::vector<IndexColumn> IndexMatrix::generate_empty_columns(Index size) {
    std::vector<IndexColumn> columns;
    for (size_t index = 0; index < size; ++index) {
        columns.emplace_back(IndexColumn(index));
    }
    return columns;
}
