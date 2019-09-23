#include <cppcon/test/async_write_stream.hpp>

#include <cstddef>
#include <cstring>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/io_context.hpp>
#include <boost/system/error_code.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace test {
namespace tests {
namespace {

TEST_CASE("async_write_some EOF",
          "[async_write_stream]")
{
  const char buffer[] = {0, 1, 2};
  bool invoked = false;
  boost::system::error_code ec;
  std::size_t bytes_transferred = 0;
  boost::asio::io_context ctx;
  async_write_stream stream(ctx.get_executor());
  CHECK(stream.empty());
  CHECK_FALSE(stream.remaining());
  stream.async_write_some(boost::asio::buffer(buffer),
                          [&](auto e,
                              auto b) noexcept
                          {
                            invoked = true;
                            ec = e;
                            bytes_transferred = b;
                          });
  CHECK_FALSE(invoked);
  REQUIRE(stream.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  stream.complete();
  CHECK_FALSE(stream.pending());
  CHECK_FALSE(invoked);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK_FALSE(ec);
  CHECK_FALSE(bytes_transferred);
  CHECK(stream.empty());
  CHECK_FALSE(stream.remaining());
}

TEST_CASE("async_write_some copy some",
          "[async_write_stream]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  std::memset(out,
              0,
              sizeof(out));
  bool invoked = false;
  boost::system::error_code ec;
  std::size_t bytes_transferred = 0;
  boost::asio::io_context ctx;
  async_write_stream stream(ctx.get_executor(),
                            out,
                            sizeof(out));
  CHECK_FALSE(stream.empty());
  CHECK(stream.remaining() == sizeof(out));
  stream.async_write_some(boost::asio::buffer(in),
                          [&](auto e,
                              auto b) noexcept
                          {
                            invoked = true;
                            ec = e;
                            bytes_transferred = b;
                          });
  CHECK_FALSE(invoked);
  REQUIRE(stream.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  stream.complete(2);
  CHECK_FALSE(stream.pending());
  CHECK_FALSE(invoked);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK_FALSE(ec);
  CHECK(bytes_transferred == 2);
  CHECK_FALSE(stream.empty());
  REQUIRE(stream.remaining() == (sizeof(out) - 2));
  CHECK(out[0] == std::byte{1});
  CHECK(out[1] == std::byte{2});
}

TEST_CASE("async_write_some copy all",
          "[async_write_stream]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  std::memset(out,
              0,
              sizeof(out));
  bool invoked = false;
  boost::system::error_code ec;
  std::size_t bytes_transferred = 0;
  boost::asio::io_context ctx;
  async_write_stream stream(ctx.get_executor(),
                            out,
                            sizeof(out));
  CHECK_FALSE(stream.empty());
  CHECK(stream.remaining() == sizeof(out));
  stream.async_write_some(boost::asio::buffer(in),
                          [&](auto e,
                              auto b) noexcept
                          {
                            invoked = true;
                            ec = e;
                            bytes_transferred = b;
                          });
  CHECK_FALSE(invoked);
  REQUIRE(stream.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  stream.complete();
  CHECK_FALSE(stream.pending());
  CHECK_FALSE(invoked);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK_FALSE(ec);
  CHECK(bytes_transferred == 3);
  CHECK_FALSE(stream.empty());
  REQUIRE(stream.remaining() == (sizeof(out) - 3));
  CHECK(out[0] == std::byte{1});
  CHECK(out[1] == std::byte{2});
  CHECK(out[2] == std::byte{3});
}

TEST_CASE("async_write_some error",
          "[async_write_stream]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  std::memset(out,
              0,
              sizeof(out));
  bool invoked = false;
  boost::system::error_code ec;
  std::size_t bytes_transferred = 0;
  boost::asio::io_context ctx;
  async_write_stream stream(ctx.get_executor(),
                            out,
                            sizeof(out));
  CHECK_FALSE(stream.empty());
  CHECK(stream.remaining() == sizeof(out));
  stream.async_write_some(boost::asio::buffer(in),
                          [&](auto e,
                              auto b) noexcept
                          {
                            invoked = true;
                            ec = e;
                            bytes_transferred = b;
                          });
  CHECK_FALSE(invoked);
  REQUIRE(stream.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  stream.complete(make_error_code(boost::system::errc::invalid_argument));
  CHECK_FALSE(stream.pending());
  CHECK_FALSE(invoked);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK(ec);
  CHECK(ec == make_error_code(boost::system::errc::invalid_argument));
  CHECK_FALSE(bytes_transferred);
  CHECK_FALSE(stream.empty());
  CHECK(stream.remaining() == sizeof(out));
}

TEST_CASE("async_write_some cancel",
          "[async_write_stream]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  std::memset(out,
              0,
              sizeof(out));
  bool invoked = false;
  boost::system::error_code ec;
  std::size_t bytes_transferred = 0;
  boost::asio::io_context ctx;
  async_write_stream stream(ctx.get_executor(),
                            out,
                            sizeof(out));
  CHECK_FALSE(stream.empty());
  CHECK(stream.remaining() == sizeof(out));
  stream.async_write_some(boost::asio::buffer(in),
                          [&](auto e,
                              auto b) noexcept
                          {
                            invoked = true;
                            ec = e;
                            bytes_transferred = b;
                          });
  CHECK_FALSE(invoked);
  REQUIRE(stream.pending());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  stream.cancel();
  CHECK_FALSE(stream.pending());
  CHECK_FALSE(invoked);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK(ec);
  CHECK(ec == make_error_code(boost::system::errc::operation_canceled));
  CHECK_FALSE(bytes_transferred);
  CHECK_FALSE(stream.empty());
  CHECK(stream.remaining() == sizeof(out));
}

TEST_CASE("async_write_some cancel none",
          "[async_write_stream]")
{
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  std::memset(out,
              0,
              sizeof(out));
  bool invoked = false;
  boost::system::error_code ec;
  std::size_t bytes_transferred = 0;
  boost::asio::io_context ctx;
  async_write_stream stream(ctx.get_executor(),
                            out,
                            sizeof(out));
  CHECK_FALSE(stream.empty());
  CHECK(stream.remaining() == sizeof(out));
  CHECK_FALSE(stream.pending());
  stream.cancel();
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK(ctx.stopped());
  CHECK_FALSE(stream.empty());
  CHECK(stream.remaining() == sizeof(out));
}

}
}
}
}
