#pragma once

#include "Norm3.h"

inline Norm3 Vec3::normalised() const noexcept {
  return Norm3(*this / length());
}

constexpr Vec3::Vec3(const Norm3 &norm)
    : x_(norm.x()), y_(norm.y()), z_(norm.z()) {}
