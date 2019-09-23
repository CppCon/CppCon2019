#include <catch2/catch.hpp>

#include "dod/Scene.h"
#include "math/ApproxVec3.h"
#include "math/Ray.h"

#include <cmath>

using dod::Scene;

namespace {

TEST_CASE("Scenes", "[Scene]") {
  SECTION("constructs") { Scene scene; }

  SECTION("intersects spheres") {
    Scene s;
    auto material = MaterialSpec::makeDiffuse(Vec3(1, 1, 1));
    s.addSphere(Vec3(10, 20, 30), 15, material);
    CHECK(!s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 1, 0))));
    CHECK(!s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(-10, -20, -30))));
    auto ir = s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(10, 20, 30)));
    REQUIRE(ir);
    CHECK(ir->material == material);
    auto &hit = ir->hit;
    CHECK(hit.distance == Approx(22.416738));
    CHECK(hit.position == ApproxVec3(5.99108, 11.9822, 17.9732));
    CHECK(hit.normal == ApproxVec3(-0.267261, -0.534522, -0.801784));
  }

  SECTION("intersect with spheres at known intersection point") {
    Scene s;
    auto material = MaterialSpec::makeDiffuse(Vec3(1, 1, 1));
    s.addSphere(Vec3(0, 0, 30), 10, material);
    auto ir = s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 2)));
    REQUIRE(ir);
    auto &hit = ir->hit;
    CHECK(hit.distance == 20);
    CHECK(hit.position == ApproxVec3(0, 0, 20));
    CHECK(hit.normal == ApproxVec3(0, 0, -1));
  }

  SECTION("intersect from within spheres at known intersection point") {
    Scene s;
    auto material = MaterialSpec::makeDiffuse(Vec3(1, 1, 1));
    s.addSphere(Vec3(0, 0, 30), 10, material);
    auto ir = s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 30), Vec3(0, 0, 2)));
    REQUIRE(ir);
    auto &hit = ir->hit;
    CHECK(hit.distance == 10);
    CHECK(hit.position == ApproxVec3(0, 0, 20));
    CHECK(hit.normal == ApproxVec3(0, 0, 1));
  }

  SECTION("picks nearer of two spheres (first nearer)") {
    Scene s;
    auto material1 = MaterialSpec::makeDiffuse(Vec3(1, 1, 1));
    auto material2 = MaterialSpec::makeDiffuse(Vec3(1, 0, 0));
    s.addSphere(Vec3(0, 0, 30), 10, material1);
    s.addSphere(Vec3(0, 0, 90), 10, material2);
    auto ir = s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 2)));
    REQUIRE(ir);
    auto &hit = ir->hit;
    CHECK(hit.distance == 20);
    CHECK(ir->material == material1);
  }

  SECTION("picks nearer of two spheres (second nearer)") {
    Scene s;
    auto material1 = MaterialSpec::makeDiffuse(Vec3(1, 1, 1));
    auto material2 = MaterialSpec::makeDiffuse(Vec3(1, 0, 0));
    s.addSphere(Vec3(0, 0, 90), 10, material1);
    s.addSphere(Vec3(0, 0, 30), 10, material2);
    auto ir = s.intersect(Ray::fromTwoPoints(Vec3(0, 0, 0), Vec3(0, 0, 2)));
    REQUIRE(ir);
    auto &hit = ir->hit;
    CHECK(hit.distance == 20);
    CHECK(ir->material == material2);
  }
  // TODO: triangles
  // TODO: mixture of triangles and spheres
}

}