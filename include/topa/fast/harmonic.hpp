#pragma once

#include "topa/common/detail/executor.hpp"
#include "topa/common/oriented_boundary_matrix.hpp"

#include "topa/models/harmonic.hpp"
#include "topa/models/filtered_complex.hpp"
#include "topa/models/separator.hpp"
#include "topa/models/solver.hpp"

#include <cassert>

namespace topa::common {

template <typename SolverImpl, typename SeparatorImpl, std::size_t percent>
class Harmonic final
    : public models::Harmonic<Harmonic<SolverImpl, SeparatorImpl, percent>> {
    static_assert(percent <= 100);
    using HarmonicPairs = std::vector<HarmonicPair>;

   public:
    template <typename Complex, PersistencePairRange R>
    static HarmonicPairs Compute(Complex&& complex, R&& pairs) {
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
        HarmonicPairs Compute(R&& persistence_pairs) const {
            HarmonicPairs pairs =
                FilterPairs(std::forward<R>(persistence_pairs));
            detail::Execute(pairs.begin(), pairs.end(),
                            [this](HarmonicPair& pair) {
                                ComputeHarmonicCycles(pair);
                            });
            return pairs;
        }

       private:
        static HarmonicCycle ConvertCycle(const VectorF& harmonic) {
            HarmonicCycle cycle;
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

        void ComputeHarmonicCycles(HarmonicPair& pair) const {
            MatrixF h_x = Solver::FindKernel(Laplacian(pair.birth));
            MatrixF h_y = Solver::FindKernel(Laplacian(pair.death - 1));
            MatrixF h_z = Solver::FindKernel(Laplacian(pair.death));

            VectorF high_representative =
                Separator::Separate(std::move(h_z), std::move(h_y));
            pair.death_edges = ConvertCycle(high_representative);
            if (pair.death_edges.empty()) {
                return;
            }
            VectorF low_representative = Separator::Separate(
                std::move(high_representative), std::move(h_x));
            pair.birth_edges = ConvertCycle(std::move(low_representative));

            const auto& edges = complex_.GetPosesBySize(2);
            if (!pair.birth_edges.empty()) {
                ComputeVertices(pair.birth_edges, pair.birth_vertices);
                pair.birth_edges.resize(edges.size());
            }
            if (!pair.death_edges.empty()) {
                ComputeVertices(pair.death_edges, pair.death_vertices);
                pair.death_edges.resize(edges.size());
            }
        }

        std::size_t FindVertexIndex(const Position& pos) const {
            const auto& vertices = complex_.GetPosesBySize(1);
            auto it = std::lower_bound(vertices.begin(), vertices.end(), pos);
            std::size_t idx = it - vertices.begin();
            assert(vertices[idx] == pos);
            return idx;
        }

        void ComputeVertices(const HarmonicCycle& edges_cycle,
                             HarmonicCycle& vertices_cycle) const {
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

        HarmonicPairs FilterPairs(
            PersistencePairRange auto&& persistence_pairs) const {
            HarmonicPairs pairs;
            for (const auto& pair : persistence_pairs) {
                assert(pair.death > pair.birth);
                if (pair.death - pair.birth > 2 &&
                    complex_.GetSizeByPos(pair.birth) == 2) {
                    pairs.emplace_back(pair.birth, pair.death);
                }
            }
            std::sort(pairs.begin(), pairs.end());
            std::size_t keep = Percent(pairs.size());
            pairs.erase(pairs.begin() + keep, pairs.end());
            return pairs;
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

       private:
        const models::FilteredComplex<ComplexImpl>& complex_;
        const EigenMatrix b_matrix_;
    };
};

};  // namespace topa::common
