#include <cppcon/cancelable_no_bases/sticky_cancel_async_write_stream.hpp>

#include <cstddef>
#include <stdexcept>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/io_context.hpp>
#include <boost/system/error_code.hpp>
#include <cppcon/test/async_write_stream.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace cancelable_no_bases {
namespace tests {
namespace {

TEST_CASE("get_executor",
          "[sticky_cancel_async_write_stream]")
{
  boost::asio::io_context ctx;
  test::async_write_stream stream(ctx.get_executor());
  sticky_cancel_async_write_stream wrapper(stream);
  CHECK(ctx.get_executor() == wrapper.get_executor());
}

TEST_CASE("Cancel async_write",
          "[sticky_cancel_async_write_stream]")
{
  bool invoked = false;
  boost::system::error_code ec;
  std::size_t bytes_transferred = 0;
  auto f = [&](auto e,
               auto b)
           {
             if (invoked) {
               throw std::logic_error("Already invoked");
             }
             invoked = true;
             ec = e;
             bytes_transferred = b;
           };
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  boost::asio::io_context ctx;
  test::async_write_stream stream(ctx.get_executor(),
                                  out,
                                  sizeof(out));
  sticky_cancel_async_write_stream wrapper(stream);
  boost::asio::async_write(wrapper,
                           boost::asio::buffer(in),
                           f);
  CHECK_FALSE(invoked);
  REQUIRE(stream.pending());
  wrapper.cancel();
  auto handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK(ec);
  CHECK(ec.default_error_condition() == make_error_code(boost::system::errc::operation_canceled));
  CHECK_FALSE(bytes_transferred);
}

TEST_CASE("Cancel async_write while handler pending",
          "[sticky_cancel_async_write_stream]")
{
  bool invoked = false;
  boost::system::error_code ec;
  std::size_t bytes_transferred = 0;
  auto f = [&](auto e,
               auto b)
           {
             if (invoked) {
               throw std::logic_error("Already invoked");
             }
             invoked = true;
             ec = e;
             bytes_transferred = b;
           };
  const char in[] = {1, 2, 3};
  std::byte out[1024];
  boost::asio::io_context ctx;
  test::async_write_stream stream(ctx.get_executor(),
                                  out,
                                  sizeof(out));
  sticky_cancel_async_write_stream wrapper(stream);
  boost::asio::async_write(wrapper,
                           boost::asio::buffer(in),
                           f);
  CHECK_FALSE(invoked);
  REQUIRE(stream.pending());
  stream.complete(1);
  wrapper.cancel();
  auto handlers = ctx.poll();
  CHECK(handlers > 1);
  CHECK(ctx.stopped());
  CHECK(invoked);
  CHECK(ec);
  CHECK(ec.default_error_condition() == make_error_code(boost::system::errc::operation_canceled));
  CHECK(bytes_transferred == 1);
  REQUIRE(stream.remaining() == (sizeof(out) - 1));
  CHECK(out[0] == std::byte{1});
}

}
}
}
}
