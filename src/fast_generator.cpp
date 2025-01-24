#include "fast_generator.h"

#include <iomanip>

State::State(const IndexMatrix& matrix) :
    m_matrix(matrix), m_lows(get_zero_vector(matrix.size())), m_arglows(get_zero_vector(matrix.size())) {};

const IndexMatrix& State::get_matrix() const {
    return m_matrix;
}

const std::vector<Index>& State::get_lows() const {
    return m_lows;
}

const std::vector<Index>& State::get_arglows() const {
    return m_arglows;
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
    for (size_t index = 0; index < m_matrix.size(); ++index) {
        m_lows[index] = m_matrix[index].lowest_element_index();
        Index low = m_lows[index];
        if (low != NO_INDEX && m_lows[low] != NO_INDEX) {
            m_lows[low] = NO_INDEX;
            std::vector<Index> representatives = m_matrix.subindices(index);
            m_matrix[low].clear_and_set_representatives(std::move(representatives));
        }
    }
}

bool State::update_arglows() {
    bool reduced = true;
    size_t irreduced_number = 0;
    for (Index index = 0; index < m_matrix.size(); ++index) {
        if (m_lows[index] != NO_INDEX) {
            if (m_arglows[m_lows[index]] == NO_INDEX) {
                m_arglows[m_lows[index]] = index;
            } else {
                reduced = false;
                ++irreduced_number;
            }
        }
    }
    std::cout << "Reduced " << std::setw(10) << m_matrix.size() - irreduced_number << "/" << m_matrix.size()
              << '\n';
    return reduced;
}

std::vector<Index> State::get_zero_vector(Index size) {
    return std::vector<Index>(size, NO_INDEX);
}
