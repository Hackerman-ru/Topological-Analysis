#include "persistence_diagram.hpp"

#include "svd.hpp"

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

    // size_t b_rows = b.rows();
    // Eigen::ColPivHouseholderQR<MatrixF> qr(std::move(b));
    // const int rank = qr.rank();

    // if (rank == 0) {
    //     return a.col(0);
    // }

    // MatrixF q = qr.matrixQ() * MatrixF::Identity(b_rows, rank);
    // MatrixF qt = q.transpose();

    // size_t best_col = 0;
    // float max_residual_norm = 0;

    // for (int i = 0; i < a.cols(); ++i) {
    //     const VectorF a_i = a.col(i);
    //     const VectorF proj = q * (q.transpose() * a_i);
    //     const VectorF residual = a_i - proj;
    //     const float residual_norm = residual.norm();

    //     if (residual_norm > max_residual_norm) {
    //         max_residual_norm = residual_norm;
    //         best_col = i;
    //     }
    // }

    // return a.col(best_col);

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
    const auto& simplices = complex.GetSimplices();
    PersistenceRepresentatives reps;
    basic_types::DefaultContainer<size_t> fitting_reps;

    auto add_pairs = [&](const Positions& poses) {
        for (const auto& pos : poses) {
            if (lows[pos] == kNonePos) {
                continue;
            }
            Position birth = lows[pos];
            Position death = pos;
            int dim = simplices[birth].simplex.size() - 1;
            if (death - birth > 1 || dim == 0) {
                if (dim == 1) {
                    fitting_reps.emplace_back(reps.size());
                }
                reps.emplace_back(birth, death, dim);
            }
        }
    };

    add_pairs(complex.GetPosesBySize(1));
    add_pairs(complex.GetPosesBySize(2));
    add_pairs(complex.GetPosesBySize(3));

    std::sort(reps.begin(), reps.end());

    EigenMatrix matrix = OrientedBoundaryMatrix(complex);

    auto spectra = [&](Position last_pos) {
        return Spectra(Laplacian(matrix, complex, last_pos));
    };

#ifdef TOPA_USE_TBB
    tbb::parallel_for_each(
        fitting_reps.begin(), fitting_reps.end(), [&](const size_t& rep_pos) {
            auto& rep = reps[rep_pos];
            MatrixF ev_bp = spectra(rep.birth - 1);
            MatrixF ev_b = spectra(rep.birth);
            MatrixF ev_dp = spectra(rep.death - 1);
            MatrixF ev_d = spectra(rep.death);

            Eigen::Index rows_bp = ev_bp.rows();
            Eigen::Index rows_b = ev_b.rows();
            if (rows_b > rows_bp) {
                ev_bp.conservativeResize(rows_b, Eigen::NoChange);
                ev_bp.bottomRows(rows_b - rows_bp).setZero();
            }

            Eigen::Index rows_dp = ev_dp.rows();
            Eigen::Index rows_d = ev_d.rows();
            if (rows_d > rows_dp) {
                ev_dp.conservativeResize(rows_d, Eigen::NoChange);
                ev_dp.bottomRows(rows_d - rows_dp).setZero();
            }

            rep.birth_harmonic_cycle =
                GetCycle(std::move(ev_b), std::move(ev_bp));
            rep.death_harmonic_cycle =
                GetCycle(std::move(ev_d), std::move(ev_dp));
        });
#else
    for (const auto& rep_pos : fitting_reps) {
        auto& rep = reps[rep_pos];
        MatrixF ev_bp = spectra(rep.birth - 1);
        MatrixF ev_b = spectra(rep.birth);
        MatrixF ev_dp = spectra(rep.death - 1);
        MatrixF ev_d = spectra(rep.death);

        ev_bp.conservativeResize(ev_bp.rows() + 1, Eigen::NoChange);
        rep.birth_harmonic_cycle = GetCycle(std::move(ev_b), std::move(ev_bp));
        rep.death_harmonic_cycle = GetCycle(std::move(ev_d), std::move(ev_dp));
    }
#endif

    return reps;
}

}  // namespace topa
