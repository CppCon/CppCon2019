#pragma once

#include "math/Hit.h"
#include "util/MaterialSpec.h"

namespace dod {

struct IntersectionRecord {
  Hit hit;
  const MaterialSpec &material;
};

}