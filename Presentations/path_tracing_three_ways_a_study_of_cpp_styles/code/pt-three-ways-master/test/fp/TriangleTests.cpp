#include <catch2/catch.hpp>

#include "fp/Triangle.h"
#include "math/ApproxVec3.h"
#include "math/Ray.h"

#include <cmath>

using namespace fp;

namespace {

TEST_CASE("Triangles", "[Triangles]") {
  SECTION("constructs") {
    Triangle t(Vec3(1, 2, 3), Vec3(2, 3, 4), Vec3(4, 5, 6));
  }
  SECTION("intersects clockwise") {
    Triangle t(Vec3(0, 0, 3), Vec3(0, 1, 3), Vec3(1, 1, 3));
    CHECK(!t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0))));
    CHECK(!t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, -1))));
    auto hit = t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 1)));
    REQUIRE(hit);
    CHECK(hit->distance == Approx(3.0));
    CHECK(hit->position == ApproxVec3(0.0, 0.0, 3.0));
    CHECK(hit->normal == ApproxVec3(0, 0, -1));
  }
  SECTION("intersects anticlockwise") {
    Triangle t(Vec3(0, 0, 3), Vec3(1, 1, 3), Vec3(0, 1, 3));
    CHECK(!t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0))));
    CHECK(!t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, -1))));
    auto hit = t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 1)));
    REQUIRE(hit);
    CHECK(hit->distance == Approx(3.0));
    CHECK(hit->position == ApproxVec3(0.0, 0.0, 3.0));
    CHECK(hit->normal == ApproxVec3(0, 0, -1));
  }

  SECTION("interpolates normals") {
    auto firstNormal = Vec3(-0.1, 0, -1).normalised();
    auto secondNormal = Vec3(0.1, 0.1, -1).normalised();
    auto thirdNormal = Vec3(-0.1, 0.1, -1).normalised();
    Triangle t(Triangle::Vertices{Vec3(0, 0, 3), Vec3(1, 1, 3), Vec3(0, 1, 3)},
               Triangle::Normals{firstNormal, secondNormal, thirdNormal});
    CHECK(!t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0))));
    CHECK(!t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, -1))));
    auto hit = t.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 1)));
    REQUIRE(hit);
    CHECK(hit->normal == ApproxVec3(-firstNormal));

    hit = t.intersect(Ray::fromTwoPoints(Vec3(1, 1, 0), Vec3(1, 1, 1)));
    REQUIRE(hit);
    CHECK(hit->normal == ApproxVec3(-secondNormal));

    hit = t.intersect(Ray::fromTwoPoints(Vec3(0, 1, 0), Vec3(0, 1, 1)));
    REQUIRE(hit);
    CHECK(hit->normal == ApproxVec3(-thirdNormal));

    hit = t.intersect(Ray::fromTwoPoints(Vec3(0.5, 0.5, 0), Vec3(0.5, 0.5, 1)));
    REQUIRE(hit);
    CHECK(hit->normal == ApproxVec3(0.000246001, -0.0498149, 0.998758));
  }
}

}