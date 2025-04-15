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
    class Builder {
       public:
        virtual ~Builder() = default;
        virtual WSimplices Build(const Pointcloud& cloud) const = 0;
    };

    class VietorisRipsBuilder : public Builder {
       public:
        VietorisRipsBuilder(Weight max_radius, size_t max_dim);

        WSimplices Build(const Pointcloud& cloud) const override;

       private:
        Weight max_radius_;
        size_t max_dim_;
    };

    class FullVietorisRipsBuilder : public Builder {
       public:
        FullVietorisRipsBuilder() = default;

        WSimplices Build(const Pointcloud& cloud) const override;
    };

   public:
    static Filtration VietorisRips(
        Weight max_radius = std::numeric_limits<Weight>::max(),
        size_t max_dim = std::numeric_limits<size_t>::max());

    static Filtration FullVietorisRips();

    WSimplices Build(const Pointcloud& cloud) const;

   private:
    explicit Filtration(std::unique_ptr<Builder> builder);

   private:
    std::unique_ptr<Builder> builder_;
};

};  // namespace topa

#endif
