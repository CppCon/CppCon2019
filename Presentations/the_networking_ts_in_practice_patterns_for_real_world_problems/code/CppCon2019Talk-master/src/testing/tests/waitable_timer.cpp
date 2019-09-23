#include <cppcon/test/waitable_timer.hpp>

#include <chrono>
#include <stdexcept>
#include <system_error>
#include <boost/asio/ts/io_context.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace test {
namespace tests {
namespace {

TEST_CASE("get_executor",
          "[waitable_timer]")
{
  boost::asio::io_context ctx;
  waitable_timer timer(ctx.get_executor());
  CHECK(timer.get_executor() == ctx.get_executor());
}


TEST_CASE("no op",
          "[waitable_timer]")
{
  boost::asio::io_context ctx;
  waitable_timer timer(ctx.get_executor());
  CHECK_FALSE(ctx.stopped());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK(ctx.stopped());
}

TEST_CASE("cancel",
          "[waitable_timer]")
{
  bool invoked = false;
  std::error_code ec;
  boost::asio::io_context ctx;
  waitable_timer timer(ctx.get_executor());
  timer.expires_after(std::chrono::duration_cast<decltype(timer)::duration>(std::chrono::seconds(1)));
  timer.async_wait([&](auto e) {
    if (invoked) {
      throw std::logic_error("Already invoked");
    }
    invoked = true;
    ec = e;
  });
  REQUIRE(timer.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  auto canceled = timer.cancel();
  CHECK(canceled == 1);
  CHECK_FALSE(invoked);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK(ec == make_error_code(std::errc::operation_canceled));
}

TEST_CASE("cancel none",
          "[waitable_timer]")
{
  boost::asio::io_context ctx;
  waitable_timer timer(ctx.get_executor());
  auto canceled = timer.cancel();
  CHECK(canceled == 0);
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK(ctx.stopped());
}

TEST_CASE("success",
          "[waitable_timer]")
{
  bool invoked = false;
  std::error_code ec;
  boost::asio::io_context ctx;
  waitable_timer timer(ctx.get_executor());
  timer.expires_at(decltype(timer)::clock_type::now());
  timer.async_wait([&](auto e) {
    if (invoked) {
      throw std::logic_error("Already invoked");
    }
    invoked = true;
    ec = e;
  });
  CHECK_FALSE(invoked);
  REQUIRE(timer.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  timer.complete();
  CHECK_FALSE(timer.pending());
  CHECK_FALSE(invoked);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK_FALSE(ec);
}

TEST_CASE("failure",
          "[waitable_timer]")
{
  bool invoked = false;
  std::error_code ec;
  boost::asio::io_context ctx;
  waitable_timer timer(ctx.get_executor());
  timer.expires_at(decltype(timer)::clock_type::now());
  timer.async_wait([&](auto e) {
    if (invoked) {
      throw std::logic_error("Already invoked");
    }
    invoked = true;
    ec = e;
  });
  CHECK_FALSE(invoked);
  REQUIRE(timer.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  timer.complete(make_error_code(std::errc::invalid_argument));
  CHECK_FALSE(timer.pending());
  CHECK_FALSE(invoked);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK(ec);
  CHECK(ec == make_error_code(std::errc::invalid_argument));
}

}
}
}
}
