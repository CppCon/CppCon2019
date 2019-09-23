#include "Norm3.h"

std::ostream &operator<<(std::ostream &o, const Norm3 &v) {
  return o << v.toVec3();
}

double Norm3::reflectance(const Norm3 &incoming, double iorFrom,
                          double iorTo) const noexcept {
  // For details, see:
  // http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
  auto iorRatio = iorFrom / iorTo;
  auto cosThetaI = -dot(incoming);
  auto sinThetaTSquared = iorRatio * iorRatio * (1 - cosThetaI * cosThetaI);
  if (sinThetaTSquared > 1) {
    // Total internal reflection.
    return 1.0;
  }
  auto cosThetaT = sqrt(1 - sinThetaTSquared);
  auto rPerpendicular = (iorFrom * cosThetaI - iorTo * cosThetaT)
                        / (iorFrom * cosThetaI + iorTo * cosThetaT);
  auto rParallel = (iorFrom * cosThetaI - iorTo * cosThetaT)
                   / (iorFrom * cosThetaI + iorTo * cosThetaT);
  return (rPerpendicular * rPerpendicular + rParallel * rParallel) / 2;
}
