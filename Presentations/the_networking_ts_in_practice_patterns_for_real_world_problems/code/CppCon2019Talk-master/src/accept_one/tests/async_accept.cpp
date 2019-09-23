#include <cppcon/accept_one/async_accept.hpp>

#include <iterator>
#include <optional>
#include <system_error>
#include <utility>
#include <boost/asio/ts/io_context.hpp>
#include <cppcon/test/async_acceptor.hpp>
#include <cppcon/test/async_write_stream.hpp>

#include <catch2/catch.hpp>

namespace cppcon::accept_one::tests {
namespace {

TEST_CASE("async_accept success with one in pool",
          "[async_accept]")
{
  std::optional<std::error_code> ec;
  boost::asio::io_context::executor_type* next = nullptr;
  boost::asio::io_context pool;
  boost::asio::io_context::executor_type ex = pool.get_executor();
  boost::asio::io_context ctx;
  using stream_type = test::async_write_stream<boost::asio::io_context::executor_type>;
  std::optional<stream_type> accepted;
  test::async_acceptor<boost::asio::io_context::executor_type,
                       stream_type> acc(ctx.get_executor());
  auto f = [&](auto e,
               auto s,
               auto n)
  {
    ec = e;
    accepted.emplace(std::move(s));
    next = n;
  };
  async_accept(acc,
               &ex,
               &ex,
               &ex + 1,
               f);
  CHECK_FALSE(ec);
  CHECK_FALSE(next);
  CHECK_FALSE(accepted);
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete();
  CHECK_FALSE(ec);
  CHECK_FALSE(next);
  CHECK_FALSE(accepted);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  REQUIRE(ec);
  CHECK_FALSE(*ec);
  CHECK(&ex == next);
  REQUIRE(accepted);
  CHECK(accepted->get_executor() == ex);
}

TEST_CASE("async_accept success with multiple in pool",
          "[async_accept]")
{
  std::optional<std::error_code> ec;
  boost::asio::io_context::executor_type* next = nullptr;
  boost::asio::io_context ctxs[2];
  boost::asio::io_context::executor_type exs[] = {ctxs[0].get_executor(),
                                                  ctxs[1].get_executor()};
  boost::asio::io_context ctx;
  using stream_type = test::async_write_stream<boost::asio::io_context::executor_type>;
  std::optional<stream_type> accepted;
  test::async_acceptor<boost::asio::io_context::executor_type,
                       stream_type> acc(ctx.get_executor());
  auto f = [&](auto e,
               auto s,
               auto n)
  {
    ec = e;
    accepted.emplace(std::move(s));
    next = n;
  };
  using std::begin;
  using std::end;
  async_accept(acc,
               begin(exs),
               begin(exs),
               end(exs),
               f);
  CHECK_FALSE(ec);
  CHECK_FALSE(next);
  CHECK_FALSE(accepted);
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete();
  CHECK_FALSE(ec);
  CHECK_FALSE(next);
  CHECK_FALSE(accepted);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  REQUIRE(ec);
  CHECK_FALSE(*ec);
  CHECK((begin(exs) + 1) == next);
  REQUIRE(accepted);
  CHECK(accepted->get_executor() == exs[0]);
  ctx.reset();
  ec.reset();
  accepted.reset();
  async_accept(acc,
               begin(exs),
               next,
               end(exs),
               f);
  CHECK_FALSE(ec);
  CHECK_FALSE(accepted);
  next = nullptr;
  handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete();
  CHECK_FALSE(ec);
  CHECK_FALSE(next);
  CHECK_FALSE(accepted);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  REQUIRE(ec);
  CHECK_FALSE(*ec);
  CHECK(begin(exs) == next);
  REQUIRE(accepted);
  CHECK(accepted->get_executor() == exs[1]);
}

TEST_CASE("async_accept failure with multiple in pool",
          "[async_accept]")
{
  std::optional<std::error_code> ec;
  boost::asio::io_context::executor_type* next = nullptr;
  boost::asio::io_context ctxs[2];
  boost::asio::io_context::executor_type exs[] = {ctxs[0].get_executor(),
                                                  ctxs[1].get_executor()};
  boost::asio::io_context ctx;
  using stream_type = test::async_write_stream<boost::asio::io_context::executor_type>;
  std::optional<stream_type> accepted;
  test::async_acceptor<boost::asio::io_context::executor_type,
                       stream_type> acc(ctx.get_executor());
  auto f = [&](auto e,
               auto s,
               auto n)
  {
    ec = e;
    accepted.emplace(std::move(s));
    next = n;
  };
  using std::begin;
  using std::end;
  async_accept(acc,
               begin(exs),
               begin(exs),
               end(exs),
               f);
  CHECK_FALSE(ec);
  CHECK_FALSE(next);
  CHECK_FALSE(accepted);
  auto handlers = ctx.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(ctx.stopped());
  REQUIRE(acc.pending());
  acc.complete(make_error_code(std::errc::invalid_argument));
  CHECK_FALSE(ec);
  CHECK_FALSE(next);
  CHECK_FALSE(accepted);
  handlers = ctx.poll();
  CHECK(handlers == 1);
  CHECK(ctx.stopped());
  REQUIRE(ec);
  CHECK(*ec);
  CHECK(*ec == make_error_code(std::errc::invalid_argument));
  CHECK((begin(exs) + 1) == next);
  REQUIRE(accepted);
  CHECK(accepted->get_executor() == exs[0]);
}

}
}
