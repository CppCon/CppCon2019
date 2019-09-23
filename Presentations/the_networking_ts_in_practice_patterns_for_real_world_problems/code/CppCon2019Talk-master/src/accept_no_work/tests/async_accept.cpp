#include <cppcon/accept_no_work/async_accept.hpp>

#include <cstddef>
#include <list>
#include <optional>
#include <system_error>
#include <utility>
#include <vector>
#include <boost/asio/ts/io_context.hpp>
#include <boost/core/noncopyable.hpp>
#include <cppcon/test/async_acceptor.hpp>
#include <cppcon/test/async_write_stream.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace accept_no_work {
namespace tests {
namespace {

TEST_CASE("async_accept round robin",
          "[async_accept]")
{
  std::optional<std::error_code> ec;
  boost::asio::io_context a;
  boost::asio::io_context b;
  boost::asio::io_context c;
  std::vector<boost::asio::io_context::executor_type> exs;
  exs.push_back(b.get_executor());
  exs.push_back(c.get_executor());
  REQUIRE(exs.size() == 2);
  using stream_type = test::async_write_stream<boost::asio::io_context::executor_type>;
  std::list<stream_type> streams;
  test::async_acceptor<boost::asio::io_context::executor_type,
                       stream_type> acc(a.get_executor());
  auto after = [&](auto stream) {
    streams.emplace_back(std::move(stream));
  };
  auto f = [&](auto e) {
    ec = e;
  };
  async_accept(acc,
               exs.begin(),
               exs.end(),
               after,
               f);
  CHECK_FALSE(ec);
  CHECK(streams.empty());
  auto handlers = a.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  b.restart();
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK(c.stopped());
  c.restart();
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
  acc.complete();
  CHECK_FALSE(ec);
  CHECK(streams.empty());
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  b.restart();
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK(c.stopped());
  c.restart();
  CHECK_FALSE(ec);
  REQUIRE(streams.size() == 1);
  CHECK(streams.front().get_executor() == b.get_executor());
  CHECK_FALSE(streams.front().remaining());
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == c.get_executor());
  acc.complete();
  CHECK_FALSE(ec);
  CHECK(streams.size() == 1);
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK(c.stopped());
  CHECK_FALSE(ec);
  REQUIRE(streams.size() == 2);
  CHECK(streams.back().get_executor() == c.get_executor());
  CHECK_FALSE(streams.back().remaining());
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
}

TEST_CASE("async_accept fail",
          "[async_accept]")
{
  std::optional<std::error_code> ec;
  boost::asio::io_context a;
  boost::asio::io_context b;
  boost::asio::io_context c;
  std::vector<boost::asio::io_context::executor_type> exs;
  exs.push_back(b.get_executor());
  exs.push_back(c.get_executor());
  REQUIRE(exs.size() == 2);
  using stream_type = test::async_write_stream<boost::asio::io_context::executor_type>;
  std::list<stream_type> streams;
  test::async_acceptor<boost::asio::io_context::executor_type,
                       stream_type> acc(a.get_executor());
  auto after = [&](auto stream) {
    streams.emplace_back(std::move(stream));
  };
  auto f = [&](auto e) {
    ec = e;
  };
  async_accept(acc,
               exs.begin(),
               exs.end(),
               after,
               f);
  CHECK_FALSE(ec);
  CHECK(streams.empty());
  auto handlers = a.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  b.restart();
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK(c.stopped());
  c.restart();
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
  acc.cancel();
  CHECK_FALSE(ec);
  CHECK(streams.empty());
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK(b.stopped());
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK(c.stopped());
  REQUIRE(ec);
  CHECK(*ec == make_error_code(std::errc::operation_canceled));
}

}
}
}
}
