#pragma once

#include <chrono>

namespace cppcon {
namespace test {

class clock {
public:
  using duration = std::chrono::nanoseconds;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = std::chrono::time_point<clock,
                                             duration>;
  static constexpr bool is_steady = false;
  static time_point now() noexcept;
  static void reset() noexcept;
  static void advance(duration d) noexcept;
  template<typename Duration>
  static void advance(Duration d) noexcept {
    advance(std::chrono::duration_cast<duration>(d));
  }
  static void set(time_point when) noexcept;
  template<typename Duration>
  static void set(std::chrono::time_point<clock,
                                          Duration> when) noexcept
  {
    set(std::chrono::time_point_cast<duration>(when));
  }
};

}
}
