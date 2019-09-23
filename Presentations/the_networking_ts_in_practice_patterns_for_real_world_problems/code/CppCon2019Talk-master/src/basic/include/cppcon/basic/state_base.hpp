#pragma once

#include <cassert>
#include <memory>
#include <optional>
#include <system_error>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/executor.hpp>

namespace cppcon {
namespace basic {

template<typename Executor,
         typename CompletionHandler>
class state_base {
  Executor                       ex_;
  CompletionHandler              handler_;
  std::optional<std::error_code> ec_;
public:
  using completion_handler_type = CompletionHandler;
  using executor_type = boost::asio::associated_executor_t<CompletionHandler,
                                                           Executor>;
  using allocator_type = boost::asio::associated_allocator_t<CompletionHandler>;
  state_base() = delete;
  state_base(const state_base&) = delete;
  state_base& operator=(const state_base&) = delete;
  state_base(const Executor& ex,
             CompletionHandler handler) noexcept(std::is_nothrow_move_constructible_v<CompletionHandler>)
    : ex_     (ex),
      handler_(std::move(handler))
  {}
  auto get_executor() const noexcept {
    return boost::asio::get_associated_executor(handler_,
                                                ex_);
  }
  auto get_allocator() const noexcept {
    return boost::asio::get_associated_allocator(handler_);
  }
  template<typename Derived>
  bool complete(const std::shared_ptr<Derived>& p,
                std::error_code ec) noexcept
  {
    assert(p.get() == this);
    if (!ec_) {
      ec_ = ec;
    }
    return p.use_count() == 1;
  }
  template<typename Derived,
           typename... Args>
  void upcall(std::shared_ptr<Derived>& p,
              Args... args) noexcept(noexcept(std::declval<CompletionHandler&>()(std::declval<std::error_code>(),
                                                                                 std::declval<Args>()...)))
  {
    assert(ec_);
    assert(p.use_count() == 1);
    assert(p.get() == this);
    auto handler = std::move(handler_);
    auto ec = *ec_;
    p.reset();
    handler(ec,
            args...);
  }
};

}
}
