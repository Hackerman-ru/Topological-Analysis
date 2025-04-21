#include "persistence_diagram.hpp"

#include "svd.hpp"

#include <format>
#include <eigen3/Eigen/SVD>
#ifdef TOPA_USE_TBB
#include <tbb/parallel_for_each.h>
#include <tbb/blocked_range.h>
#include <tbb/task_arena.h>
#endif

namespace topa {

static HarmonicCycle Convert(const VectorF& vector) {
    HarmonicCycle cycle;
    cycle.reserve(vector.size());
    for (const auto& v : vector) {
        cycle.emplace_back(v);
    }
    return cycle;
}

static VectorF AExistsInB(MatrixF&& a, MatrixF&& b) {
    if (a.size() == 0) {
        return {};
    }
    if (b.size() == 0) {
        a.col(0);
    }

    const size_t v_pos = b.cols();
    b.conservativeResize(Eigen::NoChange, v_pos + 1);
    const size_t cols = a.cols();
    size_t max_idx = 0;
    float max_sv = 0.0f;
    for (size_t i = 0; i < cols; ++i) {
        b.col(v_pos) = a.col(i);
        Eigen::BDCSVD<Eigen::MatrixXf> svd(b);
        VectorF svs = svd.singularValues();
        float min_sv = *svs.end();
        if (min_sv > max_sv) {
            max_idx = i;
            max_sv = min_sv;
        }
    }

    return a.col(max_idx);
}

static HarmonicCycle GetCycle(MatrixF&& a, MatrixF&& b) {
    return Convert(AExistsInB(std::move(a), std::move(b)));
}

PersistenceRepresentatives PersistenceDiagram(const FilteredComplex& complex,
                                              const Lows& lows) {
    PersistenceRepresentatives reps;
    basic_types::DefaultContainer<std::pair<size_t, size_t>> fitting_reps;

    auto add_pairs = [&](const Positions& poses) {
        for (const auto& pos : poses) {
            if (lows[pos] == kNonePos) {
                continue;
            }
            Position birth = lows[pos];
            Position death = pos;
            if (death < birth) {
                continue;
            }
            if (death - birth > 2 && complex.GetSizeByPos(birth) == 2) {
                reps.emplace_back(birth, death);
            }
        }
    };

    const auto& vertices = complex.GetPosesBySize(1);
    const auto& edges = complex.GetPosesBySize(2);

    add_pairs(vertices);
    add_pairs(edges);
    add_pairs(complex.GetPosesBySize(3));

    std::sort(reps.begin(), reps.end(),
              [](const PersistenceRepresentative& lhs,
                 const PersistenceRepresentative& rhs) {
                  size_t lhs_lifetime = lhs.death - lhs.birth;
                  size_t rhs_lifetime = rhs.death - rhs.birth;
                  return lhs_lifetime > rhs_lifetime;
              });

    reps.erase(reps.begin() + reps.size() / 10, reps.end());

    const size_t n = complex.Size();
    basic_types::DefaultContainer<size_t> edges_count(n);
    {
        auto it = edges.begin();
        size_t ne = 0;
        for (size_t i = 0; i < n; ++i) {
            if (it != edges.end() && *it == i) {
                ++it;
                ++ne;
            }
            edges_count[i] = ne;
        }
    }

    EigenMatrix matrix = OrientedBoundaryMatrix(complex);

    auto spectra = [&](Position last_pos) {
        return Spectra(Laplacian(matrix, complex, last_pos));
    };

    auto find_vertex_index = [&vertices](Position pos) {
        auto it = std::lower_bound(vertices.begin(), vertices.end(), pos);
        return static_cast<size_t>(it - vertices.begin());
    };

    auto update_vertices = [&](const HarmonicCycle& edges_cycle,
                               HarmonicCycle& vertices_cycle,
                               const auto& edges_list) {
        vertices_cycle.assign(vertices.size(), 0.0f);

        for (size_t i = 0; i < edges_cycle.size(); ++i) {
            const Position edge_pos = edges_list[i];
            for (Position vertex_pos : complex.GetFacetsPosition(edge_pos)) {
                const size_t idx = find_vertex_index(vertex_pos);
                vertices_cycle[idx] += edges_cycle[i];
            }
        }
    };

#ifdef TOPA_USE_TBB
    tbb::parallel_for_each(
        reps.begin(), reps.end(), [&](PersistenceRepresentative& rep) {
            MatrixF ev_bp = spectra(rep.birth - 1);
            MatrixF ev_b = spectra(rep.birth);
            // skip
            if (ev_b.size() == 0) {
                return;
            }
            Eigen::Index rows_bp = ev_bp.rows();
            Eigen::Index rows_b = ev_b.rows();
            ev_bp.conservativeResize(rows_b, Eigen::NoChange);
            ev_bp.bottomRows(rows_b - rows_bp).setZero();
            rep.birth_edges = GetCycle(std::move(ev_b), std::move(ev_bp));

            MatrixF ev_dp = spectra(rep.death - 1);
            // skip
            if (ev_dp.size() == 0) {
                rep.birth_edges.clear();
                return;
            }
            MatrixF ev_d = spectra(rep.death);
            Eigen::Index rows_dp = ev_dp.rows();
            Eigen::Index rows_d = ev_d.rows();
            ev_d.conservativeResize(rows_dp, Eigen::NoChange);
            ev_d.bottomRows(rows_dp - rows_d).setZero();
            rep.death_edges = GetCycle(std::move(ev_dp), std::move(ev_d));

            update_vertices(rep.birth_edges, rep.birth_vertices, edges);
            rep.birth_edges.resize(edges.size());
            update_vertices(rep.death_edges, rep.death_vertices, edges);
            rep.death_edges.resize(edges.size());
        });
#else
    for (const auto& rep_pos : fitting_reps) {
        auto& rep = reps[rep_pos.second];
        MatrixF ev_bp = spectra(rep.birth - 1);
        MatrixF ev_b = spectra(rep.birth);
        // skip
        if (ev_b.size() == 0) {
            continue;
        }
        Eigen::Index rows_bp = ev_bp.rows();
        Eigen::Index rows_b = ev_b.rows();
        if (rows_bp + 1 != rows_b || rows_b == 0) {
            throw std::runtime_error("Invalid birth eigvectors");
        }
        ev_bp.conservativeResize(rows_b, Eigen::NoChange);
        ev_bp.bottomRows(rows_b - rows_bp).setZero();
        rep.birth_harmonic_cycle = GetCycle(std::move(ev_b), std::move(ev_bp));

        MatrixF ev_dp = spectra(rep.death - 1);
        // skip
        if (ev_dp.size() == 0) {
            continue;
        }
        MatrixF ev_d = spectra(rep.death);
        Eigen::Index rows_dp = ev_dp.rows();
        Eigen::Index rows_d = ev_d.rows();
        if (rows_dp != rows_d || rows_d == 0) {
            throw std::runtime_error("Invalid death eigvectors");
        }
        rep.death_harmonic_cycle = GetCycle(std::move(ev_dp), std::move(ev_d));
    }
#endif

    return reps;
}

}  // namespace topa
