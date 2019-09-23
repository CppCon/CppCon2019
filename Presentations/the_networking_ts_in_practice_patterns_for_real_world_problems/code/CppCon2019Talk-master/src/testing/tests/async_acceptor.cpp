#include <cppcon/test/async_acceptor.hpp>

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <boost/asio/ts/io_context.hpp>
#include <cppcon/test/async_write_stream.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace test {
namespace tests {
namespace {

TEST_CASE("Nullary complete",
          "[async_acceptor]")
{
  std::error_code ec;
  boost::asio::io_context a;
  boost::asio::io_context b;
  using stream_type = async_write_stream<boost::asio::io_context::executor_type>;
  std::optional<stream_type> stream;
  async_acceptor<boost::asio::io_context::executor_type,
                 stream_type> acc(a.get_executor());
  auto f = [&](auto e,
               auto s)
           {
             if (stream) {
               throw std::logic_error("Already invoked");
             }
             ec = e;
             stream.emplace(std::move(s));
           };
  acc.async_accept(b.get_executor(),
                   f);
  CHECK_FALSE(stream);
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
  auto handlers = a.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  b.restart();
  acc.complete();
  CHECK_FALSE(acc.pending());
  CHECK_FALSE(stream);
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK(a.stopped());
  REQUIRE(stream);
  CHECK_FALSE(ec);
  CHECK(stream->get_executor() == b.get_executor());
  CHECK_FALSE(stream->remaining());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
}

TEST_CASE("Complete w/args (success)",
          "[async_acceptor]")
{
  std::byte buffer[1024];
  std::error_code ec;
  boost::asio::io_context a;
  boost::asio::io_context b;
  using stream_type = async_write_stream<boost::asio::io_context::executor_type>;
  std::optional<stream_type> stream;
  async_acceptor<boost::asio::io_context::executor_type,
                 stream_type> acc(a.get_executor());
  auto f = [&](auto e,
               auto s)
           {
             if (stream) {
               throw std::logic_error("Already invoked");
             }
             ec = e;
             stream.emplace(std::move(s));
           };
  acc.async_accept(b.get_executor(),
                   f);
  CHECK_FALSE(stream);
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
  auto handlers = a.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  b.restart();
  acc.complete(std::error_code(),
               buffer,
               sizeof(buffer));
  CHECK_FALSE(acc.pending());
  CHECK_FALSE(stream);
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK(a.stopped());
  REQUIRE(stream);
  CHECK_FALSE(ec);
  CHECK(stream->get_executor() == b.get_executor());
  CHECK(stream->remaining() == sizeof(buffer));
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
}

TEST_CASE("Complete w/args (failure)",
          "[async_acceptor]")
{
  std::byte buffer[1024];
  std::error_code ec;
  boost::asio::io_context a;
  boost::asio::io_context b;
  using stream_type = async_write_stream<boost::asio::io_context::executor_type>;
  std::optional<stream_type> stream;
  async_acceptor<boost::asio::io_context::executor_type,
                 stream_type> acc(a.get_executor());
  auto f = [&](auto e,
               auto s)
           {
             if (stream) {
               throw std::logic_error("Already invoked");
             }
             ec = e;
             stream.emplace(std::move(s));
           };
  acc.async_accept(b.get_executor(),
                   f);
  CHECK_FALSE(stream);
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
  auto handlers = a.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  b.restart();
  acc.complete(make_error_code(std::errc::connection_aborted),
               buffer,
               sizeof(buffer));
  CHECK_FALSE(acc.pending());
  CHECK_FALSE(stream);
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK(a.stopped());
  REQUIRE(stream);
  CHECK(ec);
  CHECK(ec == make_error_code(std::errc::connection_aborted));
  CHECK(stream->get_executor() == b.get_executor());
  CHECK(stream->remaining() == sizeof(buffer));
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
}

TEST_CASE("Cancel",
          "[async_acceptor]")
{
  std::byte buffer[1024];
  std::error_code ec;
  boost::asio::io_context a;
  boost::asio::io_context b;
  using stream_type = async_write_stream<boost::asio::io_context::executor_type>;
  std::optional<stream_type> stream;
  async_acceptor<boost::asio::io_context::executor_type,
                 stream_type> acc(a.get_executor());
  auto f = [&](auto e,
               auto s)
           {
             if (stream) {
               throw std::logic_error("Already invoked");
             }
             ec = e;
             stream.emplace(std::move(s));
           };
  acc.async_accept(b.get_executor(),
                   f);
  CHECK_FALSE(stream);
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
  auto handlers = a.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  b.restart();
  acc.cancel();
  CHECK_FALSE(acc.pending());
  CHECK_FALSE(stream);
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK(a.stopped());
  REQUIRE(stream);
  CHECK(ec);
  CHECK(ec == make_error_code(std::errc::operation_canceled));
  CHECK(stream->get_executor() == b.get_executor());
  CHECK_FALSE(stream->remaining());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
}

TEST_CASE("Accept on context",
          "[async_acceptor]")
{
  std::error_code ec;
  boost::asio::io_context a;
  boost::asio::io_context b;
  using stream_type = async_write_stream<boost::asio::io_context::executor_type>;
  std::optional<stream_type> stream;
  async_acceptor<boost::asio::io_context::executor_type,
                 stream_type> acc(a.get_executor());
  auto f = [&](auto e,
               auto s)
           {
             if (stream) {
               throw std::logic_error("Already invoked");
             }
             ec = e;
             stream.emplace(std::move(s));
           };
  acc.async_accept(b,
                   f);
  CHECK_FALSE(stream);
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
  auto handlers = a.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  b.restart();
  acc.complete();
  CHECK_FALSE(acc.pending());
  CHECK_FALSE(stream);
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK(a.stopped());
  REQUIRE(stream);
  CHECK_FALSE(ec);
  CHECK(stream->get_executor() == b.get_executor());
  CHECK_FALSE(stream->remaining());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
}

}
}
}
}
