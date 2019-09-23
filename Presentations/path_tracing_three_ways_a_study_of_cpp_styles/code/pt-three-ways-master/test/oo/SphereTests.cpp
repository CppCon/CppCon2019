#include <catch2/catch.hpp>

#include "math/ApproxVec3.h"
#include "math/Ray.h"
#include "oo/Sphere.h"

#include <cmath>

using oo::Sphere;

namespace {

TEST_CASE("Spheres", "[Sphere]") {
  SECTION("constructs") {
    Sphere s(Vec3(10, 20, 30), 15);
    CHECK(s.centre() == Vec3(10, 20, 30));
    CHECK(s.radius() == 15);
  }

  SECTION("intersects") {
    Sphere s(Vec3(10, 20, 30), 15);
    Hit hit;
    CHECK(!s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0)), hit));
    CHECK(!s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(-10, -20, -30)),
                       hit));
    REQUIRE(
        s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(10, 20, 30)), hit));
    CHECK(hit.distance == Approx(22.416738));
    CHECK(hit.position == ApproxVec3(5.99108, 11.9822, 17.9732));
    CHECK(hit.normal == ApproxVec3(-0.267261, -0.534522, -0.801784));
    CHECK(!hit.inside);
  }

  SECTION("intersect with sphere at known intersection point") {
    Sphere s(Vec3(0, 0, 30), 10);
    Hit hit;
    REQUIRE(s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 2)), hit));
    CHECK(hit.distance == 20);
    CHECK(hit.position == ApproxVec3(0, 0, 20));
    CHECK(hit.normal == ApproxVec3(0, 0, -1));
    CHECK(!hit.inside);
  }

  SECTION("intersect from within sphere at known intersection point") {
    Sphere s(Vec3(0, 0, 30), 10);
    Hit hit;
    REQUIRE(
        s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 30), Vec3(0, 0, 2)), hit));
    CHECK(hit.distance == 10);
    CHECK(hit.position == ApproxVec3(0, 0, 20));
    CHECK(hit.normal == ApproxVec3(0, 0, 1));
    CHECK(hit.inside);
  }
}

}