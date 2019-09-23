#include <cppcon/test/clock.hpp>

#include <chrono>

#include <catch2/catch.hpp>

namespace cppcon {
namespace test {
namespace tests {
namespace {

TEST_CASE("reset",
          "[clock]")
{
  clock::reset();
  CHECK_FALSE(clock::now().time_since_epoch().count());
}

TEST_CASE("advance",
          "[clock]")
{
  clock::reset();
  auto d = std::chrono::duration_cast<clock::duration>(std::chrono::seconds(2));
  clock::advance(d);
  CHECK(clock::now().time_since_epoch() == d);
  clock::advance(d);
  d += d;
  CHECK(clock::now().time_since_epoch() == d);
  clock::advance(std::chrono::seconds(1));
  d += std::chrono::duration_cast<clock::duration>(std::chrono::seconds(1));
  CHECK(clock::now().time_since_epoch() == d);
}

TEST_CASE("set",
          "[clock]")
{
  clock::reset();
  auto d = std::chrono::seconds(5);
  std::chrono::time_point<clock,
                          decltype(d)> when(d);
  clock::set(when);
  CHECK(clock::now().time_since_epoch() == std::chrono::duration_cast<clock::duration>(d));
}

}
}
}
}
