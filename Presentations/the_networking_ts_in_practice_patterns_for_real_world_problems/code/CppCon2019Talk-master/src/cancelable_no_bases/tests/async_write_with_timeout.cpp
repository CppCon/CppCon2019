#include <cppcon/cancelable_no_bases/async_write_with_timeout.hpp>

#include <chrono>
#include <cstddef>
#include <stdexcept>
#include <system_error>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/io_context.hpp>
#include <cppcon/test/async_write_stream.hpp>
#include <cppcon/test/clock.hpp>
#include <cppcon/test/waitable_timer.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace cancelable_no_bases {
namespace tests {
namespace {

class completion_handler_state {
public:
  completion_handler_state() noexcept
    : invoked          (false),
      bytes_transferred(0)
  {}
  void clear() noexcept {
    invoked = false;
    ec.clear();
    bytes_transferred = 0;
  }
  bool            invoked;
  std::error_code ec;
  std::size_t     bytes_transferred;
};

class completion_handler {
public:
  explicit completion_handler(completion_handler_state& state) noexcept
    : state_(state)
  {}
  completion_handler(completion_handler&&) = default;
  completion_handler(const completion_handler&) = delete;
  void operator()(std::error_code ec,
                  std::size_t bytes_transferred)
  {
    if (state_.invoked) {
      throw std::logic_error("Already invoked");
    }
    state_.invoked = true;
    state_.ec = ec;
    state_.bytes_transferred = bytes_transferred;
  }
private:
  completion_handler_state& state_;
};

TEST_CASE("Timeout",
          "[async_write_with_timeout]")
{
  completion_handler_state state;
  boost::asio::io_context ctx;
  test::async_write_stream stream(ctx.get_executor());
  test::waitable_timer timer(ctx.get_executor());
  auto d = std::chrono::duration_cast<decltype(timer)::duration>(std::chrono::seconds(5));
  async_write_with_timeout(stream,
                           boost::asio::const_buffer(),
                           timer,
                           d,
                           completion_handler(state));
  CHECK_FALSE(state.invoked);
  CHECK(stream.pending());
  REQUIRE(timer.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  CHECK_FALSE(state.invoked);
  timer.complete();
  CHECK_FALSE(state.invoked);
  CHECK(stream.pending());
  CHECK_FALSE(timer.pending());
  handlers = ctx.poll();
  CHECK(handlers);
  CHECK(ctx.stopped());
  CHECK(state.invoked);
  CHECK(state.ec);
  CHECK(state.ec.default_error_condition() == make_error_code(std::errc::timed_out).default_error_condition());
  CHECK_FALSE(state.bytes_transferred);
}

TEST_CASE("Timeout write already pending",
          "[async_write_with_timeout]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  completion_handler_state state;
  boost::asio::io_context ctx;
  test::async_write_stream stream(ctx.get_executor(),
                                  out,
                                  sizeof(out));
  test::waitable_timer timer(ctx.get_executor());
  auto d = std::chrono::duration_cast<decltype(timer)::duration>(std::chrono::seconds(5));
  async_write_with_timeout(stream,
                           boost::asio::buffer(in),
                           timer,
                           d,
                           completion_handler(state));
  CHECK_FALSE(state.invoked);
  REQUIRE(stream.pending());
  REQUIRE(timer.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  CHECK_FALSE(state.invoked);
  timer.complete();
  stream.complete(1);
  CHECK_FALSE(state.invoked);
  CHECK_FALSE(stream.pending());
  CHECK_FALSE(timer.pending());
  handlers = ctx.poll();
  CHECK(handlers);
  CHECK(ctx.stopped());
  CHECK(state.invoked);
  CHECK(state.ec);
  CHECK(state.ec.default_error_condition() == make_error_code(std::errc::timed_out).default_error_condition());
  CHECK(state.bytes_transferred == 1);
  REQUIRE(stream.remaining() == (sizeof(out) - 1));
  CHECK(out[0] == std::byte{1});
}

TEST_CASE("Write",
          "[async_write_with_timeout]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  completion_handler_state state;
  boost::asio::io_context ctx;
  test::async_write_stream stream(ctx.get_executor(),
                                  out,
                                  sizeof(out));
  test::waitable_timer timer(ctx.get_executor());
  auto d = std::chrono::duration_cast<decltype(timer)::duration>(std::chrono::seconds(5));
  async_write_with_timeout(stream,
                           boost::asio::buffer(in),
                           timer,
                           d,
                           completion_handler(state));
  CHECK_FALSE(state.invoked);
  REQUIRE(stream.pending());
  CHECK(timer.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  CHECK_FALSE(state.invoked);
  stream.complete();
  CHECK_FALSE(state.invoked);
  CHECK_FALSE(stream.pending());
  CHECK(timer.pending());
  handlers = ctx.poll();
  CHECK(handlers);
  CHECK(ctx.stopped());
  CHECK(state.invoked);
  CHECK_FALSE(state.ec);
  CHECK(state.bytes_transferred == sizeof(in));
  REQUIRE(stream.remaining() == (sizeof(out) - sizeof(in)));
  CHECK(out[0] == std::byte{1});
  CHECK(out[1] == std::byte{2});
  CHECK(out[2] == std::byte{3});
}

TEST_CASE("Write timeout already pending",
          "[async_write_with_timeout]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  completion_handler_state state;
  boost::asio::io_context ctx;
  test::async_write_stream stream(ctx.get_executor(),
                                  out,
                                  sizeof(out));
  test::waitable_timer timer(ctx.get_executor());
  auto d = std::chrono::duration_cast<decltype(timer)::duration>(std::chrono::seconds(5));
  async_write_with_timeout(stream,
                           boost::asio::buffer(in),
                           timer,
                           d,
                           completion_handler(state));
  CHECK_FALSE(state.invoked);
  REQUIRE(stream.pending());
  REQUIRE(timer.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  CHECK_FALSE(state.invoked);
  stream.complete();
  timer.complete();
  CHECK_FALSE(state.invoked);
  CHECK_FALSE(stream.pending());
  CHECK_FALSE(timer.pending());
  handlers = ctx.poll();
  CHECK(handlers);
  CHECK(ctx.stopped());
  CHECK(state.invoked);
  CHECK_FALSE(state.ec);
  CHECK(state.bytes_transferred == sizeof(in));
  REQUIRE(stream.remaining() == (sizeof(out) - sizeof(in)));
  CHECK(out[0] == std::byte{1});
  CHECK(out[1] == std::byte{2});
  CHECK(out[2] == std::byte{3});
}

class completion_handler_with_executor : public completion_handler {
public:
  using executor_type = boost::asio::io_context::executor_type;
  explicit completion_handler_with_executor(completion_handler_state& state,
                                            const executor_type& ex) noexcept
    : completion_handler(state),
      ex_               (ex)
  {}
  auto get_executor() const noexcept {
    return ex_;
  }
private:
  executor_type ex_;
};

TEST_CASE("Executor customization",
          "[async_write_with_timeout]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  completion_handler_state state;
  boost::asio::io_context stream_ctx;
  boost::asio::io_context timer_ctx;
  boost::asio::io_context handler_ctx;
  test::async_write_stream stream(stream_ctx.get_executor(),
                                  out,
                                  sizeof(out));
  test::waitable_timer timer(timer_ctx.get_executor());
  auto d = std::chrono::duration_cast<decltype(timer)::duration>(std::chrono::seconds(5));
  async_write_with_timeout(stream,
                           boost::asio::buffer(in),
                           timer,
                           d,
                           completion_handler_with_executor(state,
                                                            handler_ctx.get_executor()));
  CHECK_FALSE(state.invoked);
  REQUIRE(stream.pending());
  CHECK(timer.pending());
  auto handlers = stream_ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(stream_ctx.stopped());
  handlers = timer_ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(timer_ctx.stopped());
  handlers = handler_ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(handler_ctx.stopped());
  stream.complete();
  CHECK_FALSE(stream.pending());
  CHECK(timer.pending());
  handlers = stream_ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(stream_ctx.stopped());
  handlers = timer_ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(timer_ctx.stopped());
  CHECK_FALSE(state.invoked);
  handlers = handler_ctx.poll();
  CHECK(handlers == 2);
  CHECK(handler_ctx.stopped());
  CHECK(stream_ctx.stopped());
  CHECK(timer_ctx.stopped());
  CHECK_FALSE(timer.pending());
  CHECK(state.invoked);
  CHECK_FALSE(state.ec);
  CHECK(state.bytes_transferred == 3);
}

}
}
}
}
