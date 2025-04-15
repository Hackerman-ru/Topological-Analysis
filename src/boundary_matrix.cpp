// #include "boundary_matrix.hpp"
// #include "complex.hpp"
// #include "simplex_tree.hpp"

// namespace topa {

// BoundaryMatrix BoundaryMatrix::From(const Complex& complex) {
//     const auto& simplices = complex.Simplices();
//     const auto& tree = complex.Tree();

//     BoundaryMatrix matrix;
//     matrix.matrix_.reserve(simplices.size());

//     for (size_t i = 0; i < simplices.size(); ++i) {
//         const auto& simplex = simplices[i].simplex;
//         auto facets = tree.GetFacets(simplex);
//         std::sort(facets.begin(), facets.end());
//         std::vector<size_t> column;
//         for (const auto& facet : facets) {
//             size_t pos = tree.GetPosition(facet);
//             if (pos != SimplexTree::kNone) {
//                 column.push_back(pos);
//             }
//         }
//         matrix.matrix_.push_back(std::move(column));
//     }

//     return matrix;
// }

// }  // namespace topa
