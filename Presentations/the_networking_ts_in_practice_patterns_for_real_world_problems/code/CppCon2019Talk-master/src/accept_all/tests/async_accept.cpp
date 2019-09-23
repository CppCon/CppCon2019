#include <cppcon/accept_all/async_accept.hpp>

#include <iterator>
#include <optional>
#include <system_error>
#include <utility>
#include <vector>
#include <boost/asio/ts/io_context.hpp>
#include <cppcon/test/async_acceptor.hpp>
#include <cppcon/test/async_write_stream.hpp>

#include <catch2/catch.hpp>

namespace cppcon::accept_all::tests {
namespace {

TEST_CASE("async_accept single pool") {
  std::optional<std::error_code> ec;
  auto f = [&](auto e) { ec = e; };
  boost::asio::io_context pool;
  auto ex = pool.get_executor();
  boost::asio::io_context ctx;
  using stream_type = test::async_write_stream<boost::asio::io_context::executor_type>;
  std::vector<stream_type> streams;
  auto after = [&](auto s) { streams.push_back(std::move(s)); };
  test::async_acceptor<boost::asio::io_context::executor_type,
                       stream_type> acc(ctx.get_executor());
  async_accept(acc,
               &ex,
               &ex + 1,
               after,
               f);
  CHECK_FALSE(ec);
  CHECK(streams.empty());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete();
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK_FALSE(ec);
  REQUIRE(streams.size() == 1);
  CHECK(streams.front().get_executor() == ex);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete();
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK_FALSE(ec);
  CHECK(streams.size() == 2);
  CHECK(streams.back().get_executor() == ex);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete(make_error_code(std::errc::invalid_argument));
  handlers = ctx.poll();
  CHECK(handlers == 1);
  REQUIRE(ec);
  CHECK(*ec);
  CHECK(*ec == make_error_code(std::errc::invalid_argument));
  CHECK(streams.size() == 2);
  CHECK(ctx.stopped());
}

TEST_CASE("async_accept multiple pool") {
  std::optional<std::error_code> ec;
  auto f = [&](auto e) { ec = e; };
  boost::asio::io_context ctxs[2];
  boost::asio::io_context::executor_type exs[] = {ctxs[0].get_executor(),
                                                  ctxs[1].get_executor()};
  boost::asio::io_context ctx;
  using stream_type = test::async_write_stream<boost::asio::io_context::executor_type>;
  std::vector<stream_type> streams;
  auto after = [&](auto s) { streams.push_back(std::move(s)); };
  test::async_acceptor<boost::asio::io_context::executor_type,
                       stream_type> acc(ctx.get_executor());
  using std::begin;
  using std::end;
  async_accept(acc,
               begin(exs),
               end(exs),
               after,
               f);
  CHECK_FALSE(ec);
  CHECK(streams.empty());
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete();
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK_FALSE(ec);
  REQUIRE(streams.size() == 1);
  CHECK(streams.front().get_executor() == exs[0]);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete();
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK_FALSE(ec);
  CHECK(streams.size() == 2);
  CHECK(streams.back().get_executor() == exs[1]);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete();
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK_FALSE(ec);
  CHECK(streams.size() == 3);
  CHECK(streams.back().get_executor() == exs[0]);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete(make_error_code(std::errc::invalid_argument));
  handlers = ctx.poll();
  CHECK(handlers == 1);
  REQUIRE(ec);
  CHECK(*ec);
  CHECK(*ec == make_error_code(std::errc::invalid_argument));
  CHECK(streams.size() == 3);
  CHECK(ctx.stopped());
}

}
}
