#pragma once

#include "Vec3.h"

class Ray {
  Vec3 origin_;
  Norm3 direction_;

public:
  constexpr Ray(const Vec3 &origin, const Norm3 &direction) noexcept
      : origin_(origin), direction_(direction) {}

  [[nodiscard]] static Ray fromTwoPoints(const Vec3 &point1,
                                         const Vec3 &point2) {
    return Ray(point1, (point2 - point1).normalised());
  }

  [[nodiscard]] constexpr const Vec3 &origin() const noexcept {
    return origin_;
  }
  [[nodiscard]] constexpr const Norm3 &direction() const noexcept {
    return direction_;
  }

  [[nodiscard]] constexpr Vec3 positionAlong(double alongRay) const noexcept {
    return origin_ + direction_ * alongRay;
  }
};
