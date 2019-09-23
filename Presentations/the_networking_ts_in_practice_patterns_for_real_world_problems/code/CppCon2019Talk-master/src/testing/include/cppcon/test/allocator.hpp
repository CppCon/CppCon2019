#pragma once

#include <cstddef>
#include <memory>

namespace cppcon {
namespace test {

class allocator_state {
public:
  allocator_state() noexcept;
  std::size_t allocate;
  std::size_t allocate_n;
  std::size_t deallocate;
  std::size_t deallocate_n;
};

template<typename T>
class allocator : public std::allocator<T> {
private:
  using base = std::allocator<T>;
public:
  template<typename U>
  class rebind {
  public:
    using other = allocator<U>;
  };
  explicit allocator(allocator_state& state) noexcept
    : state_(&state)
  {}
  template<typename U>
  explicit allocator(const allocator<U>& other) noexcept
    : state_(&other.state())
  {}
  T* allocate(std::size_t n,
              const void* hint = nullptr)
  {
    ++state_->allocate;
    state_->allocate_n += n;
    return base::allocate(n,
                          hint);
  }
  void deallocate(T* p,
                  std::size_t n) noexcept
  {
    ++state_->deallocate;
    state_->deallocate_n += n;
    base::deallocate(p,
                     n);
  }
  allocator_state& state() const noexcept {
    return *state_;
  }
private:
  allocator_state* state_;
};

}
}
