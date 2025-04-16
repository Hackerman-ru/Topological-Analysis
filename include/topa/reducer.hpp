#ifndef TOPA_REDUCER_HPP_
#define TOPA_REDUCER_HPP_

#include "complex.hpp"
#include "matrix.hpp"
#include <memory>

namespace topa {

class Reducer {
   private:
    struct IReducer {
        virtual ~IReducer() = default;
        virtual Matrix<size_t> Reduce(const Complex& complex) const = 0;
    };

    class TwistReducer;
    class DoubleTwistReducer;

   public:
    static Reducer Twist();
    static Reducer DoubleTwist();

    Matrix<size_t> Reduce(const Complex& complex) const;

   private:
    explicit Reducer(std::unique_ptr<IReducer> strategy);

    std::unique_ptr<IReducer> strategy_;
};

}  // namespace topa

#endif
