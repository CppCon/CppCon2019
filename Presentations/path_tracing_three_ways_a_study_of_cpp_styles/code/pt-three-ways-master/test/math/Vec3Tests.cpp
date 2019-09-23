#include <catch2/catch.hpp>

#include "math/ApproxVec3.h"
#include "math/Vec3.h"

#include <cmath>
#include <sstream>

namespace {

TEST_CASE("Vectors", "[math]") {
  SECTION("default constructs to zero") {
    CHECK(Vec3().x() == 0);
    CHECK(Vec3().y() == 0);
    CHECK(Vec3().z() == 0);
  }
  SECTION("can be constructed") {
    auto vec = Vec3(1., 2., 3.);
    CHECK(vec.x() == 1.);
    CHECK(vec.y() == 2.);
    CHECK(vec.z() == 3.);
  }
  SECTION("comparison") {
    CHECK(Vec3() == Vec3());
    CHECK(Vec3() != Vec3(1, 2, 3));
    CHECK(Vec3(1, 2, 3) == Vec3(1, 2, 3));
    CHECK(Vec3(1, 2, 3) != Vec3(3, 2, 1));
  }
  SECTION("adding") {
    CHECK(Vec3() + Vec3() == Vec3());
    CHECK(Vec3(1, 1, 1) + Vec3(2, 2, 2) == Vec3(3, 3, 3));
    auto vec = Vec3(1, 2, 3);
    CHECK((vec += Vec3(2, 4, 6)) == Vec3(3, 6, 9));
    CHECK(vec == Vec3(3, 6, 9));
  }
  SECTION("subtracting") {
    CHECK(Vec3() - Vec3() == Vec3());
    CHECK(Vec3(1, 1, 1) - Vec3(2, 2, 2) == Vec3(-1, -1, -1));
    auto vec = Vec3(1, 2, 3);
    CHECK((vec -= Vec3(2, 4, 6)) == Vec3(-1, -2, -3));
    CHECK(vec == Vec3(-1, -2, -3));
  }
  SECTION("negation") {
    CHECK(-Vec3() == Vec3());
    CHECK(-Vec3(1, 2, 3) == Vec3(-1, -2, -3));
  }
  SECTION("scalar multiply") {
    CHECK(Vec3() * 0 == Vec3());
    CHECK(Vec3(1, 2, 3) * 0 == Vec3());
    CHECK(0 * Vec3(1, 2, 3) == Vec3());
    CHECK(Vec3(1, 1, 1) * 2 == Vec3(2, 2, 2));
    CHECK(2 * Vec3(1, 1, 1) == Vec3(2, 2, 2));
    auto vec = Vec3(1, 2, 3);
    CHECK((vec *= 2) == Vec3(2, 4, 6));
    CHECK(vec == Vec3(2, 4, 6));
  }
  SECTION("scalar divide") {
    CHECK(Vec3() / 1 == Vec3());
    CHECK(Vec3(1, 2, 3) / 1 == Vec3(1, 2, 3));
    CHECK(1 / Vec3(1, 2, 3) == ApproxVec3(1.0, 0.5, 0.3333333333333));
    CHECK(Vec3(1, 1, 1) / 2 == ApproxVec3(0.5, 0.5, 0.5));
    auto vec = Vec3(1, 2, 3);
    CHECK((vec /= 2) == ApproxVec3(0.5, 1, 1.5));
    CHECK(vec == ApproxVec3(0.5, 1, 1.5));
  }
  SECTION("vector multiply") {
    CHECK(Vec3() * Vec3() == Vec3());
    CHECK(Vec3(1, 2, 3) * Vec3(0, 0, 0) == Vec3());
    CHECK(Vec3(1, 2, 3) * Vec3(2, 3, 4) == Vec3(2, 6, 12));
    auto vec = Vec3(1, 2, 3);
    CHECK((vec *= Vec3(1, 2, 3)) == Vec3(1, 4, 9));
    CHECK(vec == Vec3(1, 4, 9));
  }
  SECTION("lengths") {
    CHECK(Vec3().lengthSquared() == 0);
    CHECK(Vec3().length() == 0);
    CHECK(Vec3(3, 4, 5).lengthSquared() == 50);
    CHECK(Vec3(3, 4, 5).length() == Approx(sqrt(50)));
  }
  SECTION("normalisation") {
    CHECK(Vec3(1, 0, 0).normalised() == Norm3::xAxis());
    CHECK(Vec3(10, 0, 0).normalised() == Norm3::xAxis());
    CHECK(Vec3(4, 5, 6).normalised().toVec3().length() == Approx(1.0));
  }
  SECTION("dot product") {
    CHECK(Vec3(1, 2, 3).dot(Vec3(4, 5, 6)) == Approx(1 * 4 + 2 * 5 + 3 * 6));
  }
  SECTION("cross product") {
    CHECK(Vec3(1, 2, 3).cross(Vec3(4, 5, 6)) == Vec3(-3, 6, -3));
  }
  SECTION("streaming") {
    std::ostringstream os;
    os << Vec3(5, 3, 2);
    CHECK(os.str() == "{5, 3, 2}");
  }
  SECTION("axes") {
    CHECK(Vec3::xAxis() == Vec3(1, 0, 0));
    CHECK(Vec3::yAxis() == Vec3(0, 1, 0));
    CHECK(Vec3::zAxis() == Vec3(0, 0, 1));
    CHECK(Vec3::xAxis().cross(Vec3::yAxis()) == Vec3::zAxis());
  }
}

}