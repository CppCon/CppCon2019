#include <cppcon/test/allocator.hpp>

namespace cppcon {
namespace test {

allocator_state::allocator_state() noexcept
  : allocate    (0),
    allocate_n  (0),
    deallocate  (0),
    deallocate_n(0)
{}

}
}
