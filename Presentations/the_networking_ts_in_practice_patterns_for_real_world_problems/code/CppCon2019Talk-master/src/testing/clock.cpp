#include <cppcon/test/clock.hpp>

#include <mutex>

namespace cppcon {
namespace test {

namespace {

clock::time_point tp;
std::mutex tp_m;

}

clock::time_point clock::now() noexcept {
  std::scoped_lock l(tp_m);
  return tp;
}

void clock::reset() noexcept {
  std::scoped_lock l(tp_m);
  tp = time_point();
}

void clock::advance(duration d) noexcept {
  std::scoped_lock l(tp_m);
  tp += d;
}

void clock::set(time_point when) noexcept {
  std::scoped_lock l(tp_m);
  tp = when;
}

}
}
