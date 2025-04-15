#include "complex.hpp"

namespace topa {

Complex Complex::From(const Pointcloud& cloud, const Filtration& filtration) {
    std::vector<WSimplex> wsimplices = filtration.Build(cloud);
    assert(std::is_sorted(wsimplices.begin(), wsimplices.end()));

    SimplexTree tree(cloud.Size());
    for (size_t pos = 0; pos < wsimplices.size(); ++pos) {
        tree.Add(wsimplices[pos].simplex, pos);
    }
    return Complex(std::move(wsimplices), std::move(tree));
}

}  // namespace topa
