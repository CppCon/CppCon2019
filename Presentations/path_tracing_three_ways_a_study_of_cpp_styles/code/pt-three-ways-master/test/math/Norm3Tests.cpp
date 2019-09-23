#include <catch2/catch.hpp>

#include "math/Norm3.h"

#include <cmath>
#include <sstream>

namespace {

TEST_CASE("Normals", "[math]") {
  SECTION("can be constructed") {
    auto vec = Norm3::fromNormal(Vec3(0, 0, 1));
    CHECK(vec.x() == 0.);
    CHECK(vec.y() == 0.);
    CHECK(vec.z() == 1.);
  }
  SECTION("comparison") {
    CHECK(Norm3::xAxis() == Norm3::xAxis());
    CHECK(Norm3::xAxis() != Norm3::yAxis());
  }
  SECTION("negation") {
    CHECK(-Norm3::xAxis() == Norm3::fromNormal(Vec3(-1, 0, 0)));
    CHECK(-Norm3::yAxis() == Norm3::fromNormal(Vec3(0, -1, 0)));
    CHECK(-Norm3::zAxis() == Norm3::fromNormal(Vec3(0, 0, -1)));
  }
  SECTION("dot product") {
    CHECK(Norm3::yAxis().dot(Vec3(4, 5, 6)) == Approx(5));
  }
  SECTION("cross product") {
    CHECK(Norm3::xAxis().cross(Norm3::yAxis()).normalised() == Norm3::zAxis());
  }
  SECTION("streaming") {
    std::ostringstream os;
    os << Norm3::xAxis();
    CHECK(os.str() == "{1, 0, 0}");
  }
  SECTION("axes") {
    CHECK(Norm3::xAxis() == Norm3::fromNormal(Vec3(1, 0, 0)));
    CHECK(Norm3::yAxis() == Norm3::fromNormal(Vec3(0, 1, 0)));
    CHECK(Norm3::zAxis() == Norm3::fromNormal(Vec3(0, 0, 1)));
  }
  SECTION("reflection") {
    CHECK(Norm3::xAxis().reflect(Norm3::yAxis()) == Norm3::yAxis());
    auto incoming = Vec3(1, -0.2, 0).normalised();
    CHECK(
        Norm3::yAxis().reflect(incoming)
        == Norm3::fromNormal(Vec3(incoming.x(), -incoming.y(), incoming.z())));
  }
}

}