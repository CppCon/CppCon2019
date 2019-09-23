#pragma once

#include <algorithm>
#include <cassert>
#include <optional>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/executor.hpp>
#include "pending_service.hpp"

namespace cppcon {
namespace test {

template<typename Executor,
         typename Stream>
class async_acceptor {
private:
  using signature_type = void(std::error_code,
                              Stream);
  using service_type = pending_service<signature_type>;
public:
  using executor_type = Executor;
  using stream_type = Stream;
  using stream_executor_type = decltype(std::declval<Stream&>().get_executor());
  using stream_execution_context_type = std::decay_t<decltype(std::declval<stream_executor_type&>().context())>;
  async_acceptor() = delete;
  async_acceptor(const async_acceptor&) = delete;
  async_acceptor& operator=(const async_acceptor&) = delete;
  explicit async_acceptor(const executor_type& ex) noexcept
    : ex_     (ex),
      pending_(boost::asio::use_service<service_type>(ex.context()).acquire())
  {}
  ~async_acceptor() noexcept {
    boost::asio::use_service<service_type>(ex_.context()).release(pending_);
  }
  auto get_executor() const noexcept {
    return ex_;
  }
  auto get_stream_executor() const noexcept {
    assert(stream_ex_);
    return *stream_ex_;
  }
  template<typename CompletionToken>
  auto async_accept(stream_executor_type ex,
                    CompletionToken&& token)
  {
    if (*pending_) {
      throw std::logic_error("Operation already pending");
    }
    using async_completion_type = boost::asio::async_completion<CompletionToken,
                                                                signature_type>;
    async_completion_type completion(token);
    auto assoc = boost::asio::get_associated_executor(completion.completion_handler,
                                                      get_executor());
    *pending_ = [g = boost::asio::make_work_guard(get_executor()),
                 g2 = boost::asio::make_work_guard(assoc),
                 handler = std::move(completion.completion_handler)](std::error_code ec,
                                                                     stream_type stream) mutable
                {
                  auto a = boost::asio::get_associated_allocator(handler);
                  auto ex = g2.get_executor();
                  auto f = [g = std::move(g),
                            g2 = std::move(g2),
                            handler = std::move(handler),
                            ec,
                            s = std::move(stream)]() mutable
                           {
                             handler(ec,
                                     std::move(s));
                             g.reset();
                             g2.reset();
                           };
                  ex.post(std::move(f),
                          a);
                };
    stream_ex_.emplace(ex);
    return completion.result.get();
  }
  template<typename CompletionToken>
  auto async_accept(stream_execution_context_type& ctx,
                    CompletionToken&& token)
  {
    return async_accept(ctx.get_executor(),
                        std::forward<CompletionToken>(token));
  }
  bool pending() const noexcept {
    return bool(*pending_);
  }
  void complete() {
    complete(std::error_code());
  }
  template<typename... Args>
  void complete(std::error_code ec,
                Args&&... args)
  {
    assert(pending());
    assert(stream_ex_);
    auto ex = *stream_ex_;
    stream_ex_.reset();
    (*pending_)(ec,
                stream_type(ex,
                            std::forward<Args>(args)...));
  }
  void cancel() {
    if (pending()) {
      complete(make_error_code(std::errc::operation_canceled));
    }
  }
private:
  executor_type                       ex_;
  typename service_type::pointer      pending_;
  std::optional<stream_executor_type> stream_ex_;
};

}
}
