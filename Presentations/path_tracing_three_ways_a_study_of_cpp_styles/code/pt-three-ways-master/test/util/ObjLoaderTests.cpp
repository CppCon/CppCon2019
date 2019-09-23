#include <catch2/catch.hpp>
#include <sstream>

#include "util/ObjLoader.h"

namespace {

struct ThrowingObjLoaderOpener : ObjLoaderOpener {
  std::unique_ptr<std::istream> open(const std::string &) override {
    throw std::runtime_error("Unexpected");
  }
};

struct CaptureSceneBuilder {
  struct Triangle {
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    MaterialSpec material;
  };
  std::vector<Triangle> triangles;
  void addTriangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
                   const MaterialSpec &material) {
    triangles.emplace_back(Triangle{v0, v1, v2, material});
  }
};

TEST_CASE("ObjLoader", "[ObjLoader]") {
  auto L = [](const char *text) {
    ThrowingObjLoaderOpener opener;
    std::istringstream in(text);
    CaptureSceneBuilder csb;
    loadObjFile(in, opener, csb);
    return csb;
  };
  SECTION("ignores comments and blank lines") {
    CHECK(L("").triangles.empty());
    CHECK(L("\n").triangles.empty());
    CHECK(L("  \n").triangles.empty());
    CHECK(L("  \n  ").triangles.empty());
    CHECK(L("\r").triangles.empty());
    CHECK(L("  \r").triangles.empty());
    CHECK(L("  \r  ").triangles.empty());
    CHECK(L("\r\n").triangles.empty());
    CHECK(L("  \r\n").triangles.empty());
    CHECK(L("  \r\n  ").triangles.empty());
    CHECK(L("# comment").triangles.empty());
    CHECK(L("  # comment").triangles.empty());
    CHECK(L("  # comment\n#another\n").triangles.empty());
  }
  SECTION("throws on parse errors") {
    CHECK_THROWS_WITH(L("nope"), "Unknown directive 'nope' on line 1");
    CHECK_THROWS_WITH(L("\nblargh"), "Unknown directive 'blargh' on line 2");
  }

  SECTION("parses a triangle") {
    auto res = L(R"(
v 0 0 0
v 0 0 1
v 0 1 0
f -3 -2 -1
)");
    REQUIRE(res.triangles.size() == 1);
    auto &t = res.triangles[0];
    CHECK(t.v0 == Vec3(0, 0, 0));
    CHECK(t.v1 == Vec3(0, 0, 1));
    CHECK(t.v2 == Vec3(0, 1, 0));
  }

  SECTION("parses materials") {
    std::istringstream in(R"(
newmtl leftWall
  Ns 10.0000
  Ni 1.5000
  illum 2
  Ka 0.63 0.065 0.05 # Red
  Kd 0.63 0.065 0.05
  Ks 0 0 0
  Ke 0 0 0


newmtl light
  Ns 10.0000
  Ni 1.0000
  illum 2
  Ka 0.78 0.78 0.78 # White
  Kd 0.78 0.78 0.78
  Ks 0 0 0
  Ke 17 12 4
)");
    auto res = impl::loadMaterials(in);
    CHECK(res.size() == 2);
    CHECK(res.at("leftWall").diffuse == Vec3(0.63, 0.065, 0.05));
    CHECK(res.at("leftWall").emission == Vec3());
    CHECK(res.at("light").diffuse == Vec3(0.78, 0.78, 0.78));
    CHECK(res.at("light").emission == Vec3(17, 12, 4));
  }
}

}