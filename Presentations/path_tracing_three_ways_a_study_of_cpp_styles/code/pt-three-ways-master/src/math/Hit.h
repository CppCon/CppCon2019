#pragma once

#include "Norm3.h"
#include "Vec3.h"

struct Hit {
  double distance{};
  bool inside{};
  Vec3 position;
  Norm3 normal{Norm3::xAxis()};
};
