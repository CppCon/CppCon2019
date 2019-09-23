#include "Material.h"
#include "math/OrthoNormalBasis.h"
#include "math/Samples.h"

namespace oo {

class ConcreteMaterial : public Material {
protected:
  MaterialSpec mat_;

public:
  explicit ConcreteMaterial(const MaterialSpec &mat) : mat_(mat) {}

  [[nodiscard]] Vec3 previewColour() const noexcept override {
    return mat_.diffuse;
  }

  [[nodiscard]] Vec3 totalEmission(const Vec3 &inbound) const
      noexcept override {
    return mat_.emission + inbound;
  }
};

class MatteMaterial : public ConcreteMaterial {
public:
  using ConcreteMaterial::ConcreteMaterial;
  [[nodiscard]] Vec3 sample(const Hit &hit, const Ray &incoming,
                            const RadianceSampler &radianceSampler, double u,
                            double v, double p) const override {
    double iorFrom = 1.0;
    double iorTo = mat_.indexOfRefraction;
    if (hit.inside) {
      std::swap(iorFrom, iorTo);
    }
    auto reflectivity =
        hit.normal.reflectance(incoming.direction(), iorFrom, iorTo);
    if (p < reflectivity) {
      return radianceSampler.sample(
          Ray(hit.position, coneSample(hit.normal.reflect(incoming.direction()),
                                       mat_.reflectionConeAngleRadians, u, v)));
    } else {
      auto basis = OrthoNormalBasis::fromZ(hit.normal);
      return mat_.diffuse
             * radianceSampler.sample(
                 Ray(hit.position, hemisphereSample(basis, u, v)));
    }
  }
};

class ShinyMaterial : public ConcreteMaterial {
public:
  using ConcreteMaterial::ConcreteMaterial;
  [[nodiscard]] Vec3 sample(const Hit &hit, const Ray &incoming,
                            const RadianceSampler &radianceSampler, double u,
                            double v, double p) const override {
    if (p < mat_.reflectivity) {
      return radianceSampler.sample(
          Ray(hit.position, coneSample(hit.normal.reflect(incoming.direction()),
                                       mat_.reflectionConeAngleRadians, u, v)));
    } else {
      auto basis = OrthoNormalBasis::fromZ(hit.normal);
      return mat_.diffuse
             * radianceSampler.sample(
                 Ray(hit.position, hemisphereSample(basis, u, v)));
    }
  }
};

std::unique_ptr<Material> Material::from(const MaterialSpec &mat) {
  if (mat.reflectivity >= 0) {
    return std::make_unique<ShinyMaterial>(mat);
  } else {
    return std::make_unique<MatteMaterial>(mat);
  }
}

}
