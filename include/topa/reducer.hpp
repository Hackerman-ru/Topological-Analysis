#ifndef TOPA_REDUCER_HPP_
#define TOPA_REDUCER_HPP_

#include "filtered_complex.hpp"
#include "matrix.hpp"
#include "low.hpp"
#include <memory>

namespace topa {

class Reducer {
   private:
    struct IReducer {
        virtual ~IReducer() = default;
        virtual Lows Reduce(const FilteredComplex& complex) const = 0;
    };

    class TwistReducer;
    class DoubleTwistReducer;

   public:
    static Reducer Twist();
    static Reducer DoubleTwist();

    Lows Reduce(const FilteredComplex& complex) const;

   private:
    explicit Reducer(std::unique_ptr<IReducer> strategy);

    std::unique_ptr<IReducer> strategy_;
};

}  // namespace topa

#endif
