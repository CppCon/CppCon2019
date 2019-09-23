#pragma once

#include "Primitive.h"

#include <vector>

namespace fp {

struct Scene {
  std::vector<Primitive> primitives;
  Vec3 environment;
};

}