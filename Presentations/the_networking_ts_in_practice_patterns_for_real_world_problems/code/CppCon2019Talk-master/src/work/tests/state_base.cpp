#include <cppcon/work/state_base.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/executor.hpp>
#include <boost/asio/ts/io_context.hpp>
#include <cppcon/test/allocator.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace work {
namespace tests {
namespace {

class completion_handler {};

using state_base = work::state_base<boost::asio::io_context::executor_type,
                                    completion_handler>;

static_assert(!std::is_default_constructible_v<state_base>);
static_assert(!std::is_copy_constructible_v<state_base>);
static_assert(!std::is_copy_assignable_v<state_base>);
static_assert(!std::is_move_constructible_v<state_base>);
static_assert(!std::is_move_assignable_v<state_base>);
static_assert(std::is_constructible_v<state_base,
                                      boost::asio::io_context::executor_type,
                                      completion_handler>);
static_assert(std::is_nothrow_constructible_v<state_base,
                                              boost::asio::io_context::executor_type,
                                              completion_handler>);

class throwing_completion_handler {
public:
  throwing_completion_handler(throwing_completion_handler&&);
};

using throwing_state_base = work::state_base<boost::asio::io_context::executor_type,
                                             throwing_completion_handler>;

static_assert(std::is_constructible_v<throwing_state_base,
                                      boost::asio::io_context::executor_type,
                                      throwing_completion_handler>);
static_assert(!std::is_nothrow_constructible_v<throwing_state_base,
                                               boost::asio::io_context::executor_type,
                                               throwing_completion_handler>);

using custom_executor_state_base = work::state_base<boost::asio::system_executor,
                                                    completion_handler>;

static_assert(std::is_same_v<custom_executor_state_base::executor_type,
                             boost::asio::system_executor>);

class custom_executor_completion_handler {
public:
  using executor_type = boost::asio::io_context::executor_type;
  explicit custom_executor_completion_handler(const executor_type& ex) noexcept
    : ex_(ex)
  {}
  auto get_executor() const noexcept {
    return ex_;
  }
private:
  executor_type ex_;
};

using customized_executor_state_base = work::state_base<boost::asio::system_executor,
                                                        custom_executor_completion_handler>;

static_assert(std::is_same_v<customized_executor_state_base::executor_type,
                             boost::asio::io_context::executor_type>);

TEST_CASE("Fallback executor",
          "[state_base]")
{
  boost::asio::io_context ioc;
  completion_handler handler;
  state_base state(ioc.get_executor(),
                   handler);
  CHECK(state.get_executor() == ioc.get_executor());
}

TEST_CASE("Customized executor",
          "[state_base]")
{
  boost::asio::io_context a;
  custom_executor_completion_handler handler(a.get_executor());
  boost::asio::io_context b;
  work::state_base<boost::asio::io_context::executor_type,
                   custom_executor_completion_handler> state(b.get_executor(),
                                                             handler);
  CHECK(state.get_executor() == a.get_executor());
  CHECK(state.get_executor() != b.get_executor());
}

class allocator_state_completion_handler {
public:
  explicit allocator_state_completion_handler(test::allocator_state& state) noexcept
    : state_(state)
  {}
  bool invoked() const noexcept {
    return bool(snapshot_);
  }
  const test::allocator_state& state() const noexcept {
    assert(snapshot_);
    return *snapshot_;
  }
  void operator()(std::error_code) {
    if (snapshot_) {
      throw std::logic_error("Already invoked");
    }
    snapshot_ = state_;
  }
private:
  test::allocator_state&               state_;
  std::optional<test::allocator_state> snapshot_;
};

TEST_CASE("Complete & Upcall",
          "[state_base]")
{
  boost::asio::io_context ioc;
  test::allocator_state state;
  using executor_type = boost::asio::io_context::executor_type;
  using completion_handler_type = allocator_state_completion_handler;
  using state_type = work::state_base<executor_type,
                                      std::reference_wrapper<completion_handler_type>>;
  completion_handler_type h(state);
  auto ptr = std::allocate_shared<state_type>(test::allocator<void>(state),
                                              ioc.get_executor(),
                                              std::ref(h));
  CHECK(state.allocate);
  CHECK(state.allocate_n);
  CHECK_FALSE(state.deallocate);
  CHECK_FALSE(state.deallocate_n);
  bool complete = ptr->complete(ptr,
                                std::error_code());
  REQUIRE(complete);
  REQUIRE(ptr.use_count() == 1);
  ptr->upcall(ptr);
  REQUIRE_FALSE(ptr);
  CHECK(state.allocate == state.deallocate);
  CHECK(state.allocate_n == state.deallocate_n);
  REQUIRE(h.invoked());
  CHECK(h.state().allocate == h.state().deallocate);
  CHECK(h.state().allocate_n == h.state().deallocate_n);
  CHECK(h.state().allocate == state.allocate);
  CHECK(h.state().allocate_n == state.allocate_n);
  CHECK(h.state().deallocate == state.deallocate);
  CHECK(h.state().deallocate_n == state.deallocate_n);
}

TEST_CASE("work",
          "[state_base]")
{
  class completion_handler : public custom_executor_completion_handler {
  public:
    completion_handler(const executor_type& a,
                       const executor_type& b,
                       std::optional<std::error_code>& ec,
                       bool& a_stopped,
                       bool& b_stopped) noexcept
      : custom_executor_completion_handler(a),
        b_                                (b),
        ec_                               (ec),
        a_stopped_                        (a_stopped),
        b_stopped_                        (b_stopped)
    {}
    void operator()(std::error_code ec) {
      if (ec_) {
        throw std::logic_error("Already invoked");
      }
      ec_ = ec;
      a_stopped_ = get_executor().context().stopped();
      b_stopped_ = b_.context().stopped();
    }
  private:
    executor_type                   b_;
    std::optional<std::error_code>& ec_;
    bool&                           a_stopped_;
    bool&                           b_stopped_;
  };
  std::optional<std::error_code> ec;
  bool a_stopped = true;
  bool b_stopped = true;
  boost::asio::io_context a;
  boost::asio::io_context b;
  completion_handler handler(a.get_executor(),
                             b.get_executor(),
                             ec,
                             a_stopped,
                             b_stopped);
  using state_type = work::state_base<boost::asio::io_context::executor_type,
                                      completion_handler,
                                      boost::asio::io_context::executor_type>;
  auto ptr = std::make_shared<state_type>(b.get_executor(),
                                          handler,
                                          a.get_executor());
  REQUIRE(ptr);
  auto handlers = a.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(a.stopped());
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(b.stopped());
  bool result = ptr->complete(ptr,
                              std::error_code());
  REQUIRE(result);
  auto f = [ptr = std::move(ptr)]() mutable {
    ptr->upcall(ptr);
  };
  a.get_executor().post(std::move(f),
                        std::allocator<void>());
  CHECK_FALSE(ec);
  handlers = b.poll();
  CHECK_FALSE(handlers);
  CHECK_FALSE(b.stopped());
  handlers = a.poll();
  CHECK(handlers == 1);
  CHECK(a.stopped());
  CHECK(b.stopped());
  CHECK_FALSE(a_stopped);
  CHECK_FALSE(b_stopped);
  REQUIRE(ec);
  CHECK_FALSE(*ec);
}

}
}
}
}
