#include "Sphere.h"
#include "math/Epsilon.h"

#include "optional.hpp"

using fp::Sphere;

namespace {

tl::optional<double> safeSqrt(double value) {
  return value < 0 ? tl::optional<double>() : sqrt(value);
}

}

tl::optional<Hit> Sphere::intersect(const Ray &ray) const noexcept {
  // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
  const auto op = centre_ - ray.origin();
  const auto b = op.dot(ray.direction().toVec3());
  return safeSqrt(b * b - op.lengthSquared() + radius_ * radius_)
      .and_then([&b](double determinant) -> tl::optional<double> {
        const auto minusT = b - determinant;
        const auto plusT = b + determinant;
        if (minusT < Epsilon && plusT < Epsilon)
          return tl::nullopt;
        return minusT > Epsilon ? minusT : plusT;
      })
      .map([this, &ray](double t) {
        const auto hitPosition = ray.positionAlong(t);
        const auto normal = (hitPosition - centre_).normalised();
        const bool inside = normal.dot(ray.direction()) > 0;
        return Hit{t, inside, hitPosition, inside ? -normal : normal};
      });
}
