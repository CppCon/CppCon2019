#include "Vec3.h"

#include <iostream>

std::ostream &operator<<(std::ostream &o, const Vec3 &v) {
  return o << "{" << v.x() << ", " << v.y() << ", " << v.z() << "}";
}
