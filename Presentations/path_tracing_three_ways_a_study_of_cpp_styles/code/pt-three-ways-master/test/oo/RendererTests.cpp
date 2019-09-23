#include "oo/Renderer.h"

#include <catch2/catch.hpp>
#include <iostream>

using oo::Renderer;

namespace {

TEST_CASE("Renderer", "[Renderer]") {
  SECTION("generates plausible tiles") {
    auto tiles = Renderer::generateTiles(30, 30, 10, 20, 33, 7, 1);
    // Each sample will get 30/10 = 3 x tiles, 30/20 = 2 y tiles. That's 6
    // Then we want 33 samples, with 7 samples per tile. That should be 33/7 = 5
    CHECK(tiles.size() == 6 * 5);
    for (auto &t : tiles) {
      // Each tile should be no more than 7 samples
      REQUIRE(t.samples <= 7);
      int width = t.xEnd - t.xBegin;
      REQUIRE(width > 0);
      int height = t.yEnd - t.yBegin;
      REQUIRE(height > 0);
      // Each tile should be no wider than 10
      REQUIRE(width <= 10);
      // Each tile should be no taller than 20
      REQUIRE(height <= 20);
    }
  }
}

}