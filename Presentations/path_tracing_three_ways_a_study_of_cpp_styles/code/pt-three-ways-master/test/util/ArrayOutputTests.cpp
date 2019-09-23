#include <catch2/catch.hpp>

#include <cstdio>
#include <unistd.h>

#include "util/ArrayOutput.h"

TEST_CASE("ArrayOutput", "[ArrayOutput]") {
  SECTION("Constructs sensibly") {
    ArrayOutput ao(10, 20);
    CHECK(ao.width() == 10);
    CHECK(ao.height() == 20);
    CHECK(ao.pixelAt(0, 0) == ArrayOutput::Pixel{0, 0, 0});
    CHECK(ao.rawPixelAt(0, 0) == Vec3());
  }
  SECTION("Roundtrips through a file") {
    char tempBuf[] = "/tmp/arrayoutputtestXXXXXXX";
    auto result = mkstemp(tempBuf);
    REQUIRE(result >= 0);
    ArrayOutput ao(7, 5);
    ao.addSamples(0, 0, Vec3(0.2, 0.3, 0.4), 12);
    ao.addSamples(1, 0, Vec3(0.4, 0.6, 0.7), 1);
    ao.addSamples(0, 3, Vec3(0.1, 0.2, 0.3), 2);
    ao.save(tempBuf);
    auto loaded = ArrayOutput::load(tempBuf);
    unlink(tempBuf); // TODO RAIIfy

    REQUIRE(loaded.width() == ao.width());
    REQUIRE(loaded.height() == ao.height());
    for (int y = 0; y < loaded.height(); ++y) {
      INFO("y=" << y);
      for (int x = 0; x < loaded.width(); ++x) {
        INFO("x=" << x);
        REQUIRE(loaded.rawPixelAt(x, y) == ao.rawPixelAt(x, y));
        REQUIRE(loaded.pixelAt(x, y) == ao.pixelAt(x, y));
        REQUIRE(loaded.pixelAt(x, y) == ao.pixelAt(x, y));
      }
    }
  }
}
