#ifndef TOPA_FILTRATION_HPP_
#define TOPA_FILTRATION_HPP_

#include "basic_types.hpp"
#include "pointcloud.hpp"
#include "simplex.hpp"

#include <limits>
#include <memory>

namespace topa {

class Filtration {
   private:
    struct Builder {
        virtual ~Builder() = default;
        virtual WSimplices Build(const Pointcloud& cloud) const = 0;
    };

    class FullVietorisRipsBuilder : public Builder {
       public:
        FullVietorisRipsBuilder() = default;

        WSimplices Build(const Pointcloud& cloud) const override;
    };

   public:
    static Filtration FullVietorisRips();

    WSimplices Build(const Pointcloud& cloud) const;

   private:
    explicit Filtration(std::unique_ptr<Builder> builder);

   private:
    std::unique_ptr<Builder> builder_;
};

};  // namespace topa

#endif
