#ifndef TOPA_COMPLEX_HPP_
#define TOPA_COMPLEX_HPP_

#include "filtration.hpp"
#include "simplex.hpp"
#include "simplex_tree.hpp"

namespace topa {

class Complex {
   public:
    static Complex From(const Pointcloud& cloud, const Filtration& filtration);

    const WSimplices& GetSimplices() const;
    Positions GetFacetsPosition(Position pos) const;
    Positions GetCofacetsPosition(Position pos) const;

   private:
    Complex(WSimplices wsimplices, SimplexTree tree);

    WSimplices wsimplices_;
    SimplexTree tree_;
};

}  // namespace topa

#endif
