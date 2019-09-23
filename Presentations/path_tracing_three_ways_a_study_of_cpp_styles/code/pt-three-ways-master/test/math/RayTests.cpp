#include <catch2/catch.hpp>

#include "math/Ray.h"

#include <cmath>

namespace {

TEST_CASE("Rays", "[Ray]") {
  SECTION("constructs") {
    auto r0 = Ray::fromTwoPoints(Vec3(1, 2, 3), Vec3(2, 2, 3));
    CHECK(r0.direction() == Norm3::xAxis());
    CHECK(r0.origin() == Vec3(1, 2, 3));
    auto r1 = Ray::fromTwoPoints(Vec3(1, 2, 3), Vec3(4, 5, 6));
    CHECK(r1.direction() == (Vec3(4, 5, 6) - Vec3(1, 2, 3)).normalised());
    CHECK(r1.origin() == Vec3(1, 2, 3));
  }
  SECTION("distance along") {
    auto ray = Ray::fromTwoPoints(Vec3(10, 10, 10), Vec3(10, 10, 60));
    CHECK(ray.positionAlong(0) == Vec3(10, 10, 10));
    CHECK(ray.positionAlong(50) == Vec3(10, 10, 60));
  }
}

}