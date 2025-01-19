#include "fast_generator.h"

State::State(const IndexMatrix& matrix) :
    m_matrix(matrix),
    m_lows(get_nothing_vector(matrix.size())),
    m_arglows(get_nothing_vector(matrix.size())) {};

const IndexMatrix& State::get_matrix() const {
    return m_matrix;
}

const std::vector<Index>& State::get_lows() const {
    return m_lows;
}

const std::vector<Index>& State::get_arglows() const {
    return m_arglows;
}

bool State::is_reduced() const {
    for (size_t index = 0; index < m_lows.size(); ++index) {
        if (m_lows[index] == NO_INDEX) {
            continue;
        }
        if (m_arglows[m_lows[index]] != index) {
            return false;
        }
    }
    return true;
}

IndexMatrix& State::get_matrix() {
    return m_matrix;
}

std::vector<Index>& State::get_lows() {
    return m_lows;
}

std::vector<Index>& State::get_arglows() {
    return m_arglows;
}

void State::clear_lows() {
    for (size_t index = m_lows.size(); index > 0; --index) {
        Index low = m_lows[index - 1];
        if (low != NO_INDEX && m_lows[low] != NO_INDEX) {
            m_lows[low] = NO_INDEX;
            std::vector<Index> representatives = m_matrix.subindices(index - 1);
            m_matrix[low].clear_and_set_representatives(std::move(representatives));
        }
    }
}

void State::update_lows() {
    for (Index index = 0; index < m_matrix.size(); ++index) {
        m_lows[index] = m_matrix[index].lowest_element_index();
        if (m_lows[index] != NO_INDEX && m_arglows[m_lows[index]] == NO_INDEX) {
            m_arglows[m_lows[index]] = index;
        }
    }
}

std::vector<Index> State::get_nothing_vector(Index size) {
    return std::vector<Index>(size, NO_INDEX);
}

void add_columns(Index source, Index destination, const IndexMatrix& matrix, IndexMatrix& next_matrix) {
    next_matrix[destination] = matrix[source] + matrix[destination];
}
