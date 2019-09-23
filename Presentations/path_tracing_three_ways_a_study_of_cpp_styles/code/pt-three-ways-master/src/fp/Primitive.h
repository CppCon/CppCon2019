#pragma once

#include "Sphere.h"
#include "Triangle.h"
#include "util/MaterialSpec.h"

#include <variant>

namespace fp {

struct TrianglePrimitive {
  Triangle shape;
  MaterialSpec material;
};

struct SpherePrimitive {
  Sphere shape;
  MaterialSpec material;
};

using Primitive = std::variant<TrianglePrimitive, SpherePrimitive>;

}