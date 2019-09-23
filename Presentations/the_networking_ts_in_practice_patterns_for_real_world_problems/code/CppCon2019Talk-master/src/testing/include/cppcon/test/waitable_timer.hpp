#pragma once

#include <cassert>
#include <chrono>
#include <cstddef>
#include <system_error>
#include <utility>
#include <boost/asio/ts/executor.hpp>
#include "pending_service.hpp"

namespace cppcon {
namespace test {

template<typename Executor,
         typename Clock = std::chrono::system_clock>
class waitable_timer {
private:
  using signature_type = void(std::error_code);
  using service_type = pending_service<signature_type>;
public:
  using executor_type = Executor;
  using clock_type = Clock;
  using duration = typename Clock::duration;
  using time_point = typename Clock::time_point;
  explicit waitable_timer(const executor_type& ex)
    : ex_     (ex),
      pending_(boost::asio::use_service<service_type>(ex.context()).acquire())
  {}
  ~waitable_timer() noexcept {
    boost::asio::use_service<service_type>(ex_.context()).release(pending_);
  }
  void complete(std::error_code ec = std::error_code()) {
    assert(pending());
    (*pending_)(ec);
  }
  template<typename CompletionToken>
  auto async_wait(CompletionToken&& token) {
    using async_completion_type = boost::asio::async_completion<CompletionToken,
                                                                signature_type>;
    async_completion_type completion(token);
    auto ex = boost::asio::get_associated_executor(completion.completion_handler,
                                                   get_executor());
    *pending_ = [g = boost::asio::make_work_guard(get_executor()),
                 g2 = boost::asio::make_work_guard(ex),
                 handler = std::move(completion.completion_handler)](std::error_code ec) mutable
                {
                  auto a = boost::asio::get_associated_allocator(handler);
                  auto ex = g2.get_executor();
                  auto f = [g = std::move(g),
                            g2 = std::move(g2),
                            handler = std::move(handler),
                            ec]() mutable
                           {
                             handler(ec);
                             g.reset();
                             g2.reset();
                           };
                  ex.post(std::move(f),
                          a);
                };
    return completion.result.get();
  }
  bool pending() const noexcept {
    return bool(*pending_);
  }
  std::size_t cancel() {
    if (!pending()) {
      return 0;
    }
    complete(make_error_code(std::errc::operation_canceled));
    return 1;
  }
  std::size_t expires_at(time_point when) {
    auto retr = cancel();
    when_ = when;
    return retr;
  }
  std::size_t expires_after(duration d) {
    return expires_at(Clock::now() + d);
  }
  time_point expiry() const noexcept {
    return when_;
  }
  auto get_executor() const noexcept {
    return ex_;
  }
private:
  executor_type         ex_;
  service_type::pointer pending_;
  time_point            when_;
};

}
}
