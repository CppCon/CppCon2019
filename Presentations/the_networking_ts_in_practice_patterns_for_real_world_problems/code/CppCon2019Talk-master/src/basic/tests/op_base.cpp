#include <cppcon/basic/op_base.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <boost/asio/ts/io_context.hpp>
#include <cppcon/basic/allocate_state.hpp>
#include <cppcon/basic/state_base.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace basic {
namespace tests {
namespace {

class completion_handler {
public:
  using arguments_type = std::pair<std::error_code,
                                   int>;
  bool invoked() const noexcept {
    return bool(args_);
  }
  const arguments_type& arguments() const noexcept {
    assert(args_);
    return *args_;
  }
  void operator()(std::error_code ec,
                  int i)
  {
    if (args_) {
      throw std::logic_error("Already invoked");
    }
    args_ = std::pair(ec,
                      i);
  }
private:
  std::optional<arguments_type> args_;
};

class state : public state_base<boost::asio::io_context::executor_type,
                                std::reference_wrapper<completion_handler>>
{
private:
  using base = state_base<boost::asio::io_context::executor_type,
                          std::reference_wrapper<completion_handler>>;
public:
  state(std::reference_wrapper<completion_handler> h,
        boost::asio::io_context& ioc)
    : base(ioc.get_executor(),
           std::move(h))
  {}
};

TEST_CASE("get_allocator & get_executor",
          "[op_base]")
{
  completion_handler h;
  boost::asio::io_context ioc;
  auto ptr = allocate_state<state>(std::ref(h),
                                   ioc);
  REQUIRE(ptr);
  CHECK(ptr->get_executor() == ioc.get_executor());
  CHECK(ptr->get_allocator() == std::allocator<void>());
  op_base<state> op(ptr);
  CHECK(op.get_executor() == ioc.get_executor());
  CHECK(op.get_allocator() == std::allocator<void>());
  CHECK_FALSE(h.invoked());
}

TEST_CASE("complete, reset, & upcall",
          "[op_base]")
{
  completion_handler h;
  boost::asio::io_context ioc;
  auto ptr = allocate_state<state>(std::ref(h),
                                   ioc);
  REQUIRE(ptr);
  op_base<state> op(ptr);
  REQUIRE(ptr);
  CHECK(ptr.use_count() == 2);
  op_base<state> op2(ptr);
  REQUIRE(ptr);
  CHECK(ptr.use_count() == 3);
  bool result = op2.complete(std::error_code());
  REQUIRE_FALSE(result);
  op2.reset();
  CHECK(ptr.use_count() == 2);
  ptr.reset();
  result = op.complete(make_error_code(std::errc::invalid_argument));
  REQUIRE(result);
  CHECK_FALSE(h.invoked());
  op.upcall(5);
  REQUIRE(h.invoked());
}

}
}
}
}
