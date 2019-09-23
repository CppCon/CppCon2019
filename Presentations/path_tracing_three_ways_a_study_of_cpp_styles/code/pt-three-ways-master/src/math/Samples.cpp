#include "Samples.h"
#include "Epsilon.h"

#include <cmath>

Norm3 coneSample(const Norm3 &direction, double coneTheta, double u,
                 double v) noexcept {
  if (coneTheta < Epsilon)
    return direction;
  coneTheta = coneTheta * (1.0 - (2.0 * acos(u) / M_PI));
  const auto radius = sin(coneTheta);
  const auto zScale = cos(coneTheta);
  const auto randomTheta = v * 2 * M_PI;
  const auto basis = OrthoNormalBasis::fromZ(direction);
  return basis
      .transform(
          Vec3(cos(randomTheta) * radius, sin(randomTheta) * radius, zScale))
      .normalised();
}

Norm3 hemisphereSample(const OrthoNormalBasis &basis, double u,
                       double v) noexcept {
  auto theta = 2 * M_PI * u;
  auto radiusSquared = v;
  auto radius = sqrt(radiusSquared);
  return basis
      .transform(Vec3(cos(theta) * radius, sin(theta) * radius,
                      sqrt(1 - radiusSquared)))
      .normalised();
}
