#ifndef __REDUCER_H__
#define __REDUCER_H__

#include "../core/persistence_pairing.h"
#include "index_matrix.h"

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <unordered_set>
#include <utility>

class State {
public:
    State(Index size);

    template<typename Vertex>
    State(const std::vector<WeightedSimplex<Vertex>>& weighted_simplices) :
        m_matrix(weighted_simplices),
        m_lows(get_nothing_vector(weighted_simplices.size())),
        m_arglows(get_nothing_vector(weighted_simplices.size())) {
        update_lows();
    }

    const IndexMatrix& get_matrix() const;
    const std::vector<Index>& get_lows() const;
    const std::vector<Index>& get_arglows() const;
    bool is_reduced() const;

    IndexMatrix& get_matrix();
    std::vector<Index>& get_lows();
    std::vector<Index>& get_arglows();
    void clear_lows();
    void update_lows();

private:
    static std::vector<Index> get_nothing_vector(Index size);

    IndexMatrix m_matrix;
    std::vector<Index> m_lows;
    std::vector<Index> m_arglows;
};

void add_columns(Index source, Index destination, const IndexMatrix& matrix, IndexMatrix& next_matrix);

template<typename Vertex>
State reduce(State state) {
    while (true) {
        if (state.is_reduced()) {
            break;
        }
        boost::asio::thread_pool pool(12);
        state.clear_lows();
        state.update_lows();
        const IndexMatrix& matrix = state.get_matrix();
        const std::vector<Index>& lows = state.get_lows();
        const std::vector<Index>& arglows = state.get_arglows();

        State next_state(matrix.size());
        IndexMatrix& next_matrix = next_state.get_matrix();
        std::vector<Index>& next_lows = next_state.get_lows();
        std::vector<Index>& next_arglows = next_state.get_arglows();

        for (size_t index = 0; index < matrix.size(); ++index) {
            if (lows[index] == NO_INDEX) {
                next_matrix[index] = matrix[index];
                continue;
            }
            Index min_index = arglows[lows[index]];
            if (min_index != index) {
                boost::asio::post(
                    pool, std::bind(add_columns, min_index, index, std::cref(matrix), std::ref(next_matrix)));
            } else {
                next_matrix[index] = matrix[index];
            }
        }

        pool.join();
        state = next_state;
        state.update_lows();
    }

    return state;
}

template<typename Vertex>
std::vector<Simplex<Vertex>>
    extract_representatives(const std::vector<Index>& representatives_indices,
                            const std::vector<WeightedSimplex<Vertex>>& weighted_simplices) {
    std::vector<Simplex<Vertex>> representatives;
    representatives.reserve(representatives_indices.size());
    for (const auto& index : representatives_indices) {
        representatives.push_back(weighted_simplices[index].get_simplex());
    }
    return representatives;
}

template<typename Vertex>
PersistencePairing<Vertex> extract_pairing(const State& state,
                                           const std::vector<WeightedSimplex<Vertex>>& weighted_simplices) {
    const IndexMatrix& matrix = state.get_matrix();
    const std::vector<Index>& lows = state.get_lows();
    PersistencePairing<Vertex> persistence_pairing;
    std::unordered_set<Index> essentials_indices;
    std::vector<std::pair<Index, Index>> pairs_indices;

    for (size_t index = 0; index < matrix.size(); ++index) {
        if (lows[index] == NO_INDEX) {
            essentials_indices.insert(index);
            continue;
        }
        essentials_indices.erase(lows[index]);
        pairs_indices.emplace_back(std::make_pair(lows[index], index));
    }

    for (const auto& essential_index : essentials_indices) {
        WeightedSimplex<Vertex> essential = weighted_simplices[essential_index];
        const std::vector<Index>& representatives_indices = matrix[essential_index].representatives();
        std::vector<Simplex<Vertex>> representatives =
            extract_representatives(representatives_indices, weighted_simplices);
        persistence_pairing.add_essential(Essential<Vertex> {.weighted_simplex = std::move(essential),
                                                             .representatives = std::move(representatives)});
    }

    for (const auto& [creator_index, destroyer_index] : pairs_indices) {
        WeightedSimplex<Vertex> creator = weighted_simplices[creator_index];
        WeightedSimplex<Vertex> destroyer = weighted_simplices[destroyer_index];
        const std::vector<Index>& representatives_indices = matrix[creator_index].representatives();
        std::vector<Simplex<Vertex>> representatives =
            extract_representatives(representatives_indices, weighted_simplices);
        persistence_pairing.add_pair(PersistencePair<Vertex> {.creator = std::move(creator),
                                                              .destroyer = std::move(destroyer),
                                                              .representatives = std::move(representatives)});
    }

    return persistence_pairing;
}

template<typename Vertex>
PersistencePairing<Vertex>
    generate_persistence_pairing(const std::vector<WeightedSimplex<Vertex>>& weighted_simplices) {
    State state = reduce<Vertex>(State(weighted_simplices));
    return extract_pairing<Vertex>(state, weighted_simplices);
}

#endif
