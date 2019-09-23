#pragma once

#include "math/Vec3.h"

namespace dod {

struct Sphere {
  Vec3 centre;
  double radiusSquared;

  Sphere(const Vec3 &centre, double radius)
      : centre(centre), radiusSquared(radius * radius) {}
};

}