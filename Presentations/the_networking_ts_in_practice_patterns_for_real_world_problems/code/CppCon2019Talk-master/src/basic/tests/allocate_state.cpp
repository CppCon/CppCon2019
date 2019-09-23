#include <cppcon/basic/allocate_state.hpp>

#include <boost/asio/ts/io_context.hpp>
#include <cppcon/basic/state_base.hpp>
#include <cppcon/test/allocator.hpp>

#include <catch2/catch.hpp>

namespace cppcon {
namespace basic {
namespace tests {
namespace {

class completion_handler {
public:
  explicit completion_handler(test::allocator_state& state) noexcept
    : state_(state)
  {}
  using allocator_type = test::allocator<void>;
  allocator_type get_allocator() const noexcept {
    return allocator_type(state_);
  }
private:
  test::allocator_state& state_;
};

TEST_CASE("allocate_state",
          "[allocate_state]")
{
  boost::asio::io_context ioc;
  test::allocator_state state;
  completion_handler h(state);
  using state_type = state_base<boost::asio::io_context::executor_type,
                                completion_handler>;
  auto ptr = allocate_state<state_type>(ioc.get_executor(),
                                        h);
  REQUIRE(ptr);
  CHECK(ptr->get_executor() == ioc.get_executor());
  CHECK(state.allocate);
  CHECK(state.allocate_n);
  CHECK_FALSE(state.deallocate);
  CHECK_FALSE(state.deallocate_n);
  auto before = state.allocate;
  auto before_n = state.allocate_n;
  ptr.reset();
  CHECK(before == state.allocate);
  CHECK(before_n == state.allocate_n);
  CHECK(state.allocate == state.deallocate);
  CHECK(state.allocate_n == state.deallocate_n);
}

}
}
}
}
