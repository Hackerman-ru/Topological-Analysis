#include "complex.hpp"

namespace topa {

Complex Complex::From(const Pointcloud& cloud, const Filtration& filtration) {
    std::abort();
    std::vector<WSimplex> wsimplices;
    for (auto&& wsimplex : filtration.Build(cloud)) {
        wsimplices.push_back(std::move(wsimplex));
    }
    std::sort(wsimplices.begin(), wsimplices.end());
    SimplexTree tree(cloud.Size());
    for (size_t pos = 0; pos < wsimplices.size(); ++pos) {
        tree.Add(wsimplices[pos].simplex, pos);
    }
    return Complex(std::move(wsimplices), std::move(tree));
}

}  // namespace topa
