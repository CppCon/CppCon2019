#include <cppcon/test/allocator.hpp>

#include <memory>
#include <type_traits>

#include <catch2/catch.hpp>

namespace cppcon {
namespace test {
namespace tests {
namespace {

static_assert(std::is_same_v<allocator<int>,
                             std::allocator_traits<allocator<double>>::rebind_alloc<int>>);
static_assert(std::is_nothrow_constructible_v<std::allocator<int>,
                                              const std::allocator<double>>);

TEST_CASE("allocator::allocate & ::deallocate",
          "[allocator]")
{
  allocator_state state;
  allocator<int> alloc(state);
  int* ptr = alloc.allocate(2);
  alloc.deallocate(ptr,
                   2);
  CHECK(state.allocate == 1);
  CHECK(state.allocate_n == 2);
  CHECK(state.deallocate == 1);
  CHECK(state.deallocate_n == 2);
}

}
}
}
}
