#include <cppcon/test/pending_service.hpp>

#include <cassert>
#include <stdexcept>
#include <boost/asio/ts/io_context.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace test {
namespace tests {
namespace {

TEST_CASE("acquire",
          "[pending_service]")
{
  boost::asio::io_context ctx;
  pending_service<void()> service(ctx);
  auto handle = service.acquire();
  CHECK_FALSE(*handle);
}

class tester {
public:
  tester(bool& invoked,
         bool& destroyed) noexcept
    : invoked_  (&invoked),
      destroyed_(&destroyed)
  {}
  ~tester() noexcept {
    assert(destroyed_);
    *destroyed_ = true;
  }
  void operator()() {
    assert(invoked_);
    if (*invoked_) {
      throw std::logic_error("Already invoked");
    }
    *invoked_ = true;
  }
private:
  bool* invoked_;
  bool* destroyed_;
};

TEST_CASE("release",
          "[pending_service]")
{
  bool invoked = false;
  bool destroyed = false;
  boost::asio::io_context ctx;
  pending_service<void()> service(ctx);
  auto handle = service.acquire();
  *handle = tester(invoked,
                   destroyed);
  destroyed = false;
  service.release(handle);
  CHECK(destroyed);
  CHECK_FALSE(invoked);
}

TEST_CASE("invoke",
          "[pending_service]")
{
  bool invoked = false;
  bool destroyed = false;
  boost::asio::io_context ctx;
  pending_service<void()> service(ctx);
  auto handle = service.acquire();
  *handle = tester(invoked,
                   destroyed);
  destroyed = false;
  (*handle)();
  CHECK(invoked);
  CHECK(destroyed);
}

}
}
}
}
