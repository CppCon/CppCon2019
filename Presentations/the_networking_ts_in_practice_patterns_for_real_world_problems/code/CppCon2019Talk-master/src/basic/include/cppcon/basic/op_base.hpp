#pragma once

#include <cassert>
#include <memory>
#include <system_error>
#include <utility>

namespace cppcon {
namespace basic {

template<typename State>
class op_base {
  std::shared_ptr<State> state_;
public:
  using executor_type = typename State::executor_type;
  using allocator_type = typename State::allocator_type;
  explicit op_base(std::shared_ptr<State> state) noexcept
    : state_(std::move(state))
  {}
  op_base(op_base&&) = default;
  op_base& operator=(op_base&&) = default;
  op_base(const op_base&) = delete;
  op_base& operator=(const op_base&) = delete;
  auto get_executor() const noexcept {
    assert(state_);
    return state_->get_executor();
  }
  auto get_allocator() const noexcept {
    assert(state_);
    return state_->get_allocator();
  }
  bool complete(std::error_code ec) noexcept {
    assert(state_);
    return state_->complete(state_,
                            ec);
  }
  void reset() noexcept {
    state_.reset();
  }
  template<typename... Args>
  void upcall(Args&&... args) noexcept(noexcept(std::declval<State&>().upcall(std::declval<std::shared_ptr<State>&>(),
                                                                              std::forward<Args>(args)...)))
  {
    assert(state_);
    state_->upcall(state_,
                   std::forward<Args>(args)...);
  }
  State& state() const noexcept {
    assert(state_);
    return *state_;
  }
};

}
}
