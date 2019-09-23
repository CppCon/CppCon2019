#include <cppcon/accept/async_accept.hpp>

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
namespace accept {
namespace tests {
namespace {

class executor_state : private boost::noncopyable {
public:
  executor_state() noexcept
    : started (0),
      finished(0)
  {}
  std::size_t started;
  std::size_t finished;
};

class execution_context;

class executor : public boost::asio::io_context::executor_type {
private:
  using base = boost::asio::io_context::executor_type;
public:
  using base::base;
  executor(const base& other,
           executor_state& state)
    : base  (other),
      state_(state)
  {}
  void on_work_started() const noexcept {
    base::on_work_started();
    ++state_.started;
  }
  void on_work_finished() const noexcept {
    base::on_work_finished();
    ++state_.finished;
  }
  execution_context& context() const noexcept;
private:
  executor_state& state_;
};

class execution_context : public boost::asio::io_context {
private:
  using base = boost::asio::io_context;
public:
  explicit execution_context(executor_state& state)
    : state_(state)
  {}
  using executor_type = executor;
  auto get_executor() noexcept {
    return executor(base::get_executor(),
                    state_);
  }
private:
  executor_state& state_;
};

execution_context& executor::context() const noexcept {
  return static_cast<execution_context&>(base::context());
}

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
  CHECK_FALSE(b.stopped());
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(c.stopped());
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
  CHECK_FALSE(b.stopped());
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(c.stopped());
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
  CHECK_FALSE(b.stopped());
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(c.stopped());
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
  executor_state state;
  std::optional<std::error_code> ec;
  execution_context a(state);
  execution_context b(state);
  execution_context c(state);
  std::vector<executor> exs;
  exs.push_back(b.get_executor());
  exs.push_back(c.get_executor());
  REQUIRE(exs.size() == 2);
  using stream_type = test::async_write_stream<executor>;
  std::list<stream_type> streams;
  test::async_acceptor<executor,
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
  CHECK_FALSE(b.stopped());
  handlers = c.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(c.stopped());
  REQUIRE(acc.pending());
  CHECK(acc.get_stream_executor() == b.get_executor());
  acc.cancel();
  CHECK_FALSE(ec);
  CHECK(streams.empty());
  CHECK(state.started > state.finished);
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK(state.started == state.finished);
  CHECK(a.stopped());
  CHECK(b.stopped());
  CHECK(c.stopped());
  REQUIRE(ec);
  CHECK(*ec == make_error_code(std::errc::operation_canceled));
}

}
}
}
}
