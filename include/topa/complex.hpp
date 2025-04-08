#ifndef TOPA_COMPLEX_HPP_
#define TOPA_COMPLEX_HPP_

#include "filtration.hpp"
#include "simplex.hpp"
#include "simplex_tree.hpp"

namespace topa {

class Complex {
   public:
    static Complex From(const Pointcloud& cloud, const Filtration& filtration);

   private:
    Complex(std::vector<WSimplex> wsimplices, SimplexTree tree)
        : wsimplices_(std::move(wsimplices)),
          tree_(std::move(tree)) {
    }

    std::vector<WSimplex> wsimplices_;
    SimplexTree tree_;
};

}  // namespace topa

#endif
