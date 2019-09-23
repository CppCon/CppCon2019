#include <catch2/catch.hpp>

#include "dod/Scene.h"
#include "math/ApproxVec3.h"
#include "math/Ray.h"

#include <cmath>

namespace {

TEST_CASE("Triangles", "[Triangles]") {
  dod::Scene scene;
  MaterialSpec mat;
  auto inf = std::numeric_limits<double>::infinity();
  SECTION("intersects clockwise") {
    scene.addTriangle(Vec3(0, 0, 3), Vec3(0, 1, 3), Vec3(1, 1, 3), mat);
    CHECK(!scene.intersectTriangles(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0)), inf));
    CHECK(!scene.intersectTriangles(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, -1)), inf));
    auto ir = scene.intersectTriangles(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 1)), inf);
    REQUIRE(ir);
    CHECK(ir->hit.distance == Approx(3.0));
    CHECK(ir->hit.position == ApproxVec3(0.0, 0.0, 3.0));
    CHECK(ir->hit.normal == ApproxVec3(0, 0, -1));
    CHECK(!scene.intersectTriangles(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 1)), 2.999));
  }
  SECTION("intersects anticlockwise") {
    scene.addTriangle(Vec3(0, 0, 3), Vec3(1, 1, 3), Vec3(0, 1, 3), mat);
    CHECK(!scene.intersectTriangles(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0)), inf));
    CHECK(!scene.intersectTriangles(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, -1)), inf));
    auto ir = scene.intersectTriangles(
        Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 1)), inf);
    REQUIRE(ir);
    CHECK(ir->hit.distance == Approx(3.0));
    CHECK(ir->hit.position == ApproxVec3(0.0, 0.0, 3.0));
    CHECK(ir->hit.normal == ApproxVec3(0, 0, -1));
  }
}

}