#pragma once

#include "common/detail/executor.hpp"
#include "common/oriented_boundary_matrix.hpp"

#include "models/harmonic.hpp"
#include "models/filtered_complex.hpp"
#include "models/separator.hpp"
#include "models/solver.hpp"

#include <cassert>

namespace topa::common {

template <typename SolverImpl, typename SeparatorImpl, std::size_t percent>
class Harmonic final
    : public models::Harmonic<Harmonic<SolverImpl, SeparatorImpl, percent>> {
    static_assert(percent <= 100);
    using HarmonicCycles = std::vector<HarmonicCycle>;

   public:
    template <typename Complex, PersistencePairRange R>
    static HarmonicCycles Compute(Complex&& complex, R&& pairs) {
        Wrapper wrapper(std::forward<Complex>(complex));
        return wrapper.Compute(std::forward<R>(pairs));
    }

   private:
    template <typename ComplexImpl>
    class Wrapper {
        using MatrixF = Eigen::MatrixXf;
        using VectorF = Eigen::VectorXf;
        using Solver = models::Solver<SolverImpl>;
        using Separator = models::Separator<SeparatorImpl>;

       public:
        explicit Wrapper(const models::FilteredComplex<ComplexImpl>& complex)
            : complex_(complex),
              b_matrix_(OrientedBoundaryMatrix<ComplexImpl>(complex)) {
        }

        template <PersistencePairRange R>
        HarmonicCycles Compute(R&& pairs) const {
            HarmonicCycles cycles = FilterPairs(std::forward<R>(pairs));
            detail::Execute(
                cycles.begin(), cycles.end(), [this](HarmonicCycle& cycle) {
                    cycle.birth_edges = ComputeCycle(cycle.birth, true);
                    cycle.death_edges = ComputeCycle(cycle.death, false);
                    UpdateCycle(cycle);
                });
            return cycles;
        }

       private:
        static HarmonicCycle::Cycle ConvertCycle(const VectorF& harmonic) {
            HarmonicCycle::Cycle cycle;
            cycle.reserve(harmonic.size());
            std::ranges::copy(harmonic, std::back_inserter(cycle));
            return cycle;
        }

        static std::size_t Percent(std::size_t value) {
            std::size_t result = (value * percent + 99) / 100;
            if (result == 0) {
                result = 1;
            }
            return result;
        }

        std::size_t FindVertexIndex(const Position& pos) const {
            const auto& vertices = complex_.GetPosesBySize(1);
            auto it = std::lower_bound(vertices.begin(), vertices.end(), pos);
            std::size_t idx = it - vertices.begin();
            assert(vertices[idx] == pos);
            return idx;
        }

        void ComputeVertices(const HarmonicCycle::Cycle& edges_cycle,
                             HarmonicCycle::Cycle& vertices_cycle) const {
            const auto& vertices = complex_.GetPosesBySize(1);
            const auto& edges = complex_.GetPosesBySize(2);
            vertices_cycle.assign(vertices.size(), 0.0f);

            for (size_t i = 0; i < edges_cycle.size(); ++i) {
                const Position edge_pos = edges[i];
                auto facets_pos = complex_.GetFacetsPosition(edge_pos);
                for (const Position& vertex_pos : facets_pos) {
                    const size_t idx = FindVertexIndex(vertex_pos);
                    vertices_cycle[idx] += edges_cycle[i];
                }
            }
        }

        void UpdateCycle(HarmonicCycle& cycle) const {
            const auto& edges = complex_.GetPosesBySize(2);
            if (!cycle.birth_edges.empty()) {
                ComputeVertices(cycle.birth_edges, cycle.birth_vertices);
                cycle.birth_edges.resize(edges.size());
            }
            if (!cycle.death_edges.empty()) {
                ComputeVertices(cycle.death_edges, cycle.death_vertices);
                cycle.death_edges.resize(edges.size());
            }
        }

        HarmonicCycles FilterPairs(PersistencePairRange auto&& pairs) const {
            HarmonicCycles cycles;
            for (const auto& pair : pairs) {
                assert(pair.death > pair.birth);
                if (pair.death - pair.birth > 2 &&
                    complex_.GetSizeByPos(pair.birth) == 2) {
                    cycles.emplace_back(pair.birth, pair.death);
                }
            }
            std::sort(cycles.begin(), cycles.end());
            std::size_t keep = Percent(cycles.size());
            cycles.erase(cycles.begin() + keep, cycles.end());
            return cycles;
        }

        std::vector<Position> CutIndices(std::size_t size,
                                         std::size_t until) const {
            std::vector<Position> result;
            for (const auto& pos : complex_.GetPosesBySize(size)) {
                if (pos > until) {
                    break;
                }
                result.emplace_back(pos);
            }
            return result;
        }

        EigenMatrix CutMatrix(int order, std::size_t until) const {
            const auto sizes =
                order == 1 ? std::make_pair(1, 2) : std::make_pair(2, 3);
            std::vector<size_t> rows = CutIndices(sizes.first, until);
            std::vector<size_t> cols = CutIndices(sizes.second, until);

            EigenMatrix submatrix(rows.size(), cols.size());
            for (size_t j = 0; j < cols.size(); ++j) {
                for (EigenMatrix::InnerIterator it(b_matrix_, cols[j]); it;
                     ++it) {
                    auto row_it =
                        std::lower_bound(rows.begin(), rows.end(), it.row());
                    if (row_it != rows.end() &&
                        *row_it == static_cast<size_t>(it.row())) {
                        size_t new_row = row_it - rows.begin();
                        submatrix.insert(new_row, j) = it.value();
                    }
                }
            }
            return submatrix;
        }

        EigenMatrix Laplacian(std::size_t until) const {
            EigenMatrix b0 = CutMatrix(1, until);
            EigenMatrix b1 = CutMatrix(2, until);

            EigenMatrix b0t = b0.transpose();
            EigenMatrix b1t = b1.transpose();

            EigenMatrix l1 = b0t * b0 + b1 * b1t;
            return l1;
        }

        HarmonicCycle::Cycle ComputeCycle(Position pos, bool birth) const {
            MatrixF eigvectors_prev = Solver::FindKernel(Laplacian(pos - 1));
            MatrixF eigvectors = Solver::FindKernel(Laplacian(pos));

            auto rows_prev = eigvectors_prev.rows();
            auto rows = eigvectors.rows();

            if (birth) {
                if (rows != 0 && rows_prev != 0) {
                    assert(rows_prev + 1 == rows);
                    eigvectors_prev.conservativeResize(rows, Eigen::NoChange);
                    eigvectors_prev.bottomRows(rows - rows_prev).setZero();
                }
                VectorF harmonic = Separator::Separate(
                    std::move(eigvectors_prev), std::move(eigvectors));
                return ConvertCycle(harmonic);
            } else {
                if (rows != 0 && rows_prev != 0) {
                    assert(rows_prev == rows);
                }
                VectorF harmonic = Separator::Separate(
                    std::move(eigvectors), std::move(eigvectors_prev));
                return ConvertCycle(harmonic);
            }
        }

       private:
        const models::FilteredComplex<ComplexImpl>& complex_;
        const EigenMatrix b_matrix_;
    };
};

};  // namespace topa::common
