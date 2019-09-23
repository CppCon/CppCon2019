#include <catch2/catch.hpp>

#include "fp/Sphere.h"
#include "math/ApproxVec3.h"
#include "math/Ray.h"

#include <cmath>

using fp::Sphere;

namespace {

TEST_CASE("Spheres", "[Sphere]") {
  SECTION("constructs") {
    Sphere s(Vec3(10, 20, 30), 15);
    CHECK(s.centre() == Vec3(10, 20, 30));
    CHECK(s.radius() == 15);
  }

  SECTION("intersects") {
    Sphere s(Vec3(10, 20, 30), 15);
    CHECK(!s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0))));
    CHECK(!s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(-10, -20, -30))));
    auto hit = s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(10, 20, 30)));
    REQUIRE(hit);
    CHECK(hit->distance == Approx(22.416738));
    CHECK(hit->position == ApproxVec3(5.99108, 11.9822, 17.9732));
    CHECK(hit->normal == ApproxVec3(-0.267261, -0.534522, -0.801784));
    CHECK(!hit->inside);
  }

  SECTION("intersect with sphere at known intersection point") {
    Sphere s(Vec3(0, 0, 30), 10);
    auto hit = s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 2)));
    REQUIRE(hit);
    CHECK(hit->distance == 20);
    CHECK(hit->position == ApproxVec3(0, 0, 20));
    CHECK(hit->normal == ApproxVec3(0, 0, -1));
    CHECK(!hit->inside);
  }

  SECTION("intersect from within sphere at known intersection point") {
    Sphere s(Vec3(0, 0, 30), 10);
    auto hit = s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 30), Vec3(0, 0, 2)));
    REQUIRE(hit);
    CHECK(hit->distance == 10);
    CHECK(hit->position == ApproxVec3(0, 0, 20));
    CHECK(hit->normal == ApproxVec3(0, 0, 1));
    CHECK(hit->inside);
  }
}

}