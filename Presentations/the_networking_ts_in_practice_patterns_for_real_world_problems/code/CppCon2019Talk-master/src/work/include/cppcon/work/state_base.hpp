#pragma once

#include <cassert>
#include <memory>
#include <optional>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/executor.hpp>

namespace cppcon {
namespace work {

template<typename... Executors>
using work_guards_t = std::tuple<boost::asio::executor_work_guard<Executors>...>;

template<typename... Executors>
auto make_work_guards(const Executors&... exs) noexcept {
  return std::tuple(boost::asio::executor_work_guard(exs)...);
}

template<typename Executor,
         typename CompletionHandler,
         typename... Executors>
class state_base {
public:
  using completion_handler_type = CompletionHandler;
  using executor_type = boost::asio::associated_executor_t<CompletionHandler,
                                                           Executor>;
private:
  CompletionHandler              handler_;
  std::optional<std::error_code> ec_;
  work_guards_t<Executor,
                executor_type,
                Executors...> gs_;
public:
  using allocator_type = boost::asio::associated_allocator_t<CompletionHandler>;
  state_base() = delete;
  state_base(const state_base&) = delete;
  state_base& operator=(const state_base&) = delete;
  state_base(const Executor& ex,
             CompletionHandler handler,
             const Executors&... exs) noexcept(std::is_nothrow_move_constructible_v<CompletionHandler>)
    : handler_(std::move(handler)),
      gs_     (work::make_work_guards(ex,
                                      boost::asio::get_associated_executor(handler_,
                                                                           ex),
                                      exs...))
  {}
  auto get_executor() const noexcept {
    return std::get<1>(gs_).get_executor();
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
    auto gs = std::move(gs_);
    p.reset();
    handler(ec,
            args...);
  }
};

}
}
