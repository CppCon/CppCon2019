#include <catch2/catch.hpp>

#include "dod/Scene.h"
#include "math/ApproxVec3.h"
#include "math/Ray.h"

#include <cmath>

namespace {

TEST_CASE("Spheres", "[Sphere]") {
  dod::Scene scene;
  MaterialSpec mat;
  auto inf = std::numeric_limits<double>::infinity();
  SECTION("intersects") {
    scene.addSphere(Vec3(10, 20, 30), 15, mat);
    CHECK(!scene.intersectSpheres(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0)), inf));
    CHECK(!scene.intersectSpheres(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(-10, -20, -30)), inf));
    auto ir = scene.intersectSpheres(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(10, 20, 30)), inf);
    REQUIRE(ir);
    CHECK(ir->hit.distance == Approx(22.416738));
    CHECK(ir->hit.position == ApproxVec3(5.99108, 11.9822, 17.9732));
    CHECK(ir->hit.normal == ApproxVec3(-0.267261, -0.534522, -0.801784));
    CHECK(!ir->hit.inside);
    CHECK(!scene.intersectSpheres(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(10, 20, 30)), 22.0));
  }

  SECTION("intersect with sphere at known intersection point") {
    scene.addSphere(Vec3(0, 0, 30), 10, mat);
    auto ir = scene.intersectSpheres(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 2)), inf);
    REQUIRE(ir);
    CHECK(ir->hit.distance == 20);
    CHECK(ir->hit.position == ApproxVec3(0, 0, 20));
    CHECK(ir->hit.normal == ApproxVec3(0, 0, -1));
    CHECK(!ir->hit.inside);
  }

  SECTION("intersect from within sphere at known intersection point") {
    scene.addSphere(Vec3(0, 0, 30), 10, mat);
    auto ir = scene.intersectSpheres(
        Ray::fromTwoPoints(Vec3(0, 0, 30), Vec3(0, 0, 2)), inf);
    REQUIRE(ir);
    CHECK(ir->hit.distance == 10);
    CHECK(ir->hit.position == ApproxVec3(0, 0, 20));
    CHECK(ir->hit.normal == ApproxVec3(0, 0, 1));
    CHECK(ir->hit.inside);
  }
}

}