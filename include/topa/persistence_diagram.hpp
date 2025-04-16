#ifndef TOPA_PERSISTENCE_DIAGRAM_HPP_
#define TOPA_PERSISTENCE_DIAGRAM_HPP_

#include "complex.hpp"
#include "matrix.hpp"

namespace topa {

class PersistenceDiagram {
   public:
    static PersistenceDiagram From(
        const Complex& complex, const Matrix<size_t>& reduced_boundary_matrix);

   private:
};

};  // namespace topa

#endif
