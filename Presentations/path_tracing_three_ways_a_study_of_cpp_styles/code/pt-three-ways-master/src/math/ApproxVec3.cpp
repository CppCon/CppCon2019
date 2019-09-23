#include "ApproxVec3.h"

#include <iostream>

std::ostream &operator<<(std::ostream &o, const ApproxVec3 &v) {
  return o << "Approx" << v.vec_;
}