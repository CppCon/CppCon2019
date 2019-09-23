#pragma once

#include "math/Hit.h"
#include "math/Ray.h"
#include "util/MaterialSpec.h"

#include <memory>

namespace oo {

class Renderer;

class Material {

public:
  virtual ~Material() = default;

  class RadianceSampler {
  public:
    virtual ~RadianceSampler() = default;

    [[nodiscard]] virtual Vec3 sample(const Ray &ray) const = 0;
  };

  [[nodiscard]] virtual Vec3 sample(const Hit &hit, const Ray &incoming,
                                    const RadianceSampler &radianceSampler,
                                    double u, double v, double p) const = 0;

  [[nodiscard]] virtual Vec3 previewColour() const noexcept = 0;

  [[nodiscard]] virtual Vec3 totalEmission(const Vec3 &inbound) const
      noexcept = 0;

  static std::unique_ptr<Material> from(const MaterialSpec &mat);
};

}
