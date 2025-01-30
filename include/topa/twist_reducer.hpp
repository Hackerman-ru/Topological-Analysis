#ifndef TOPA_DOUBLE_TWIST_REDUCER_HPP
#define TOPA_DOUBLE_TWIST_REDUCER_HPP

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace topa {
    template<typename Matrix, typename FastColumn, typename Filtration, typename index_t = size_t>
    class DoubleTwistReducer {
    public:
        static Matrix reduce(const Filtration& filtration) {
            std::cout << "Max simplex size: " << filtration.max_simplex_size() << '\n';
            std::cout << "Generating coboundary matrix...\n";
            Matrix coboundary_matrix = generate_coboundary_matrix(filtration);
            std::cout << "Reducing coboundary matrix...\n";
            auto start = std::chrono::high_resolution_clock::now();
            reduce_coboundary_matrix(coboundary_matrix, filtration);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            std::cout << "\e[1mTime taken by coboundary matrix reduction: "
                      << static_cast<double>(duration.count()) / 1000 << " seconds\e[0m" << std::endl;
            std::cout << "Computing saved simplices...\n";
            auto saved_simplices = compute_saved_simplices(std::move(coboundary_matrix));
            std::cout << "Saved simplices number: " << saved_simplices.size() << '\n';
            std::cout << "Generating boundary matrix...\n";
            Matrix boundary_matrix = generate_boundary_matrix(filtration, std::move(saved_simplices));
            std::cout << "Reducing boundary matrix...\n";
            start = std::chrono::high_resolution_clock::now();
            reduce_boundary_matrix(boundary_matrix, filtration);
            stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            std::cout << "\e[1mTime taken by boundary matrix reduction: "
                      << static_cast<double>(duration.count()) / 1000 << " seconds\e[0m" << std::endl;
            return boundary_matrix;
        }

    private:
        static Matrix generate_coboundary_matrix(const Filtration& filtration) {
            const auto& indexer = filtration.indexer();
            const auto& simplices = filtration.simplices();
            Matrix coboundary_matrix(simplices.size());
            for (index_t index = 0; index < simplices.size(); ++index) {
                for (const auto& subsimplex : simplices[index].subsimplices()) {
                    coboundary_matrix[indexer.at(subsimplex)].emplace_back(index);
                }
            }
            return coboundary_matrix;
        }

        static Matrix generate_boundary_matrix(const Filtration& filtration,
                                               std::vector<index_t>&& saved_simplices) {
            const auto& indexer = filtration.indexer();
            const auto& simplices = filtration.simplices();
            Matrix boundary_matrix(simplices.size());
            for (const auto& index : saved_simplices) {
                for (const auto& subsimplex : simplices[index].subsimplices()) {
                    boundary_matrix[index].emplace_back(indexer.at(subsimplex));
                }
            }
            return boundary_matrix;
        }

        static void reduce_coboundary_matrix(Matrix& coboundary_matrix, const Filtration& filtration) {
            const auto& simplices = filtration.simplices();
            const size_t max_simplex_size = filtration.max_simplex_size();
            const index_t n = coboundary_matrix.size();
            static constexpr index_t none = -1;
            std::vector<index_t> arglow(n, none);
            FastColumn fast_column(n);
            for (size_t d = 1; d < max_simplex_size; ++d) {
                for (index_t index = 0; index < n; ++index) {
                    if (simplices[index].size() != d) {
                        continue;
                    }
                    if (coboundary_matrix[index].empty()) {
                        continue;
                    }
                    fast_column.add(coboundary_matrix[index]);
                    index_t low_index = fast_column.get_max_index();
                    while (!fast_column.empty() && arglow[low_index] != none) {
                        fast_column.add(coboundary_matrix[arglow[low_index]]);
                        low_index = fast_column.get_max_index();
                    }
                    if (!fast_column.empty()) {
                        arglow[low_index] = index;
                        coboundary_matrix[low_index].clear();
                    }
                    coboundary_matrix[index] = fast_column.get_indices_and_clear();
                }
            }
        }

        static void reduce_boundary_matrix(Matrix& boundary_matrix, const Filtration& filtration) {
            const auto& simplices = filtration.simplices();
            const size_t max_simplex_size = filtration.max_simplex_size();
            const index_t n = boundary_matrix.size();
            std::vector<index_t> arglow(n, 0);
            FastColumn fast_column(n);
            for (size_t d = max_simplex_size; d > 1; --d) {
                for (index_t index = 0; index < n; ++index) {
                    if (simplices[index].size() != d) {
                        continue;
                    }
                    if (boundary_matrix[index].empty()) {
                        continue;
                    }
                    fast_column.add(boundary_matrix[index]);
                    index_t low_index = fast_column.get_max_index();
                    while (!fast_column.empty() && arglow[low_index] != 0) {
                        fast_column.add(boundary_matrix[arglow[low_index]]);
                        low_index = fast_column.get_max_index();
                    }
                    if (!fast_column.empty()) {
                        arglow[low_index] = index;
                        boundary_matrix[low_index].clear();
                    }
                    boundary_matrix[index] = fast_column.get_indices_and_clear();
                }
            }
        }

        static std::vector<index_t> compute_saved_simplices(Matrix&& reduced_coboundary_matrix) {
            const index_t n = reduced_coboundary_matrix.size();
            FastColumn fast_column(n);
            std::vector<index_t> saved_simplices;
            for (index_t index = 0; index < n; ++index) {
                if (!reduced_coboundary_matrix[index].empty()) {
                    fast_column.add(reduced_coboundary_matrix[index]);
                    saved_simplices.emplace_back(n - 1 - fast_column.get_max_index());
                    fast_column.clear();
                }
            }
            return saved_simplices;
        }
    };
}   // namespace topa

#endif
