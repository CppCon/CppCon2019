#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/executor.hpp>
#include <boost/system/error_code.hpp>
#include "pending_service.hpp"

namespace cppcon {
namespace test {

template<typename Executor>
class async_write_stream {
private:
  using signature_type = void(boost::system::error_code,
                              std::size_t);
  using service_type = pending_service<signature_type>;
public:
  using executor_type = Executor;
  async_write_stream() = delete;
  async_write_stream(const async_write_stream&) = delete;
  async_write_stream& operator=(const async_write_stream&) = delete;
  async_write_stream(async_write_stream&& other)
    : async_write_stream(other.ex_,
                         other.out_,
                         other.size_)
  {
    assert(!other.pending());
  }
  async_write_stream(const executor_type& ex,
                     std::byte* out,
                     std::size_t size) noexcept
    : ex_     (ex),
      out_    (out),
      size_   (size),
      pending_(boost::asio::use_service<service_type>(ex.context()).acquire())
  {}
  explicit async_write_stream(const executor_type& ex) noexcept
    : async_write_stream(ex,
                         nullptr,
                         0)
  {}
  ~async_write_stream() noexcept {
    boost::asio::use_service<service_type>(ex_.context()).release(pending_);
  }
  auto get_executor() const noexcept {
    return ex_;
  }
  template<typename ConstBufferSequence,
           typename CompletionToken>
  auto async_write_some(ConstBufferSequence cb,
                        CompletionToken&& token)
  {
    if (*pending_) {
      throw std::logic_error("Operation already pending");
    }
    using async_completion_type = boost::asio::async_completion<CompletionToken,
                                                                signature_type>;
    async_completion_type completion(token);
    auto ex = boost::asio::get_associated_executor(completion.completion_handler,
                                                   get_executor());
    *pending_ = [this,
                 g = boost::asio::make_work_guard(get_executor()),
                 g2 = boost::asio::make_work_guard(ex),
                 handler = std::move(completion.completion_handler),
                 cb](boost::system::error_code ec,
                     std::size_t bytes_transferred) mutable
                {
                  bytes_transferred = boost::asio::buffer_copy(boost::asio::buffer(out_,
                                                                                   size_),
                                                               cb,
                                                               bytes_transferred);
                  out_ += bytes_transferred;
                  size_ -= bytes_transferred;
                  auto a = boost::asio::get_associated_allocator(handler);
                  auto ex = g2.get_executor();
                  auto f = [g = std::move(g),
                            g2 = std::move(g2),
                            handler = std::move(handler),
                            ec,
                            bytes_transferred]() mutable
                           {
                             handler(ec,
                                     bytes_transferred);
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
  std::size_t remaining() const noexcept {
    return size_;
  }
  bool empty() const noexcept {
    return !remaining();
  }
  void complete(boost::system::error_code ec,
                std::size_t bytes_transferred)
  {
    assert(pending());
    (*pending_)(ec,
                bytes_transferred);
  }
  void complete() {
    complete(boost::system::error_code(),
             std::numeric_limits<std::size_t>::max());
  }
  void complete(boost::system::error_code ec) {
    complete(ec,
             0);
  }
  void complete(std::size_t bytes_transferred) {
    complete(boost::system::error_code(),
             bytes_transferred);
  }
  void cancel() {
    if (pending()) {
      complete(make_error_code(boost::system::errc::operation_canceled));
    }
  }
private:
  executor_type         ex_;
  std::byte*            out_;
  std::size_t           size_;
  service_type::pointer pending_;
};

}
}
