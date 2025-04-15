// #include "coboundary_matrix.hpp"
// #include "complex.hpp"
// #include "simplex_tree.hpp"

// namespace topa {

// CoboundaryMatrix CoboundaryMatrix::From(const Complex& complex) {
//     const auto& simplices = complex.Simplices();
//     const auto& tree = complex.Tree();

//     CoboundaryMatrix matrix;
//     matrix.matrix_.resize(simplices.size());

//     for (size_t j = 0; j < simplices.size(); ++j) {
//         const auto& simplex_j = simplices[j].simplex;
//         auto cofacets = tree.GetCofacets(simplex_j);
//         std::sort(cofacets.begin(), cofacets.end());
//         for (const auto& cofacet : cofacets) {
//             if (cofacet.size() != simplex_j.size() + 1) {
//                 continue;
//             }
//             size_t i = tree.GetPosition(cofacet);
//             if (i != SimplexTree::kNone) {
//                 matrix.matrix_[j].push_back(i);
//             }
//         }
//     }

//     return matrix;
// }

// }  // namespace topa
