#pragma once

#include <cstddef>
#include <memory>
#include <system_error>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/executor.hpp>
#include <boost/core/noncopyable.hpp>

namespace cppcon {
namespace basic_no_bases {

template<class AsyncWriteStream,
         class ConstBuffers,
         class Timer,
         class Token>
auto async_write_with_timeout(AsyncWriteStream& stream,
                              ConstBuffers cb,
                              Timer& timer,
                              typename Timer::duration timeout,
                              Token&& token)
{
  using signature_type = void(std::error_code,
                              std::size_t);
  using completion_type = boost::asio::async_completion<Token,
                                                        signature_type>;
  using handler_type = typename completion_type::completion_handler_type;
  struct state : private boost::noncopyable {
    state(AsyncWriteStream& s,
          Timer& t,
          handler_type h) noexcept(std::is_nothrow_move_constructible_v<handler_type>)
      : stream (s),
        timer  (t),
        handler(std::move(h))
    {}
    AsyncWriteStream& stream;
    Timer&            timer;
    handler_type      handler;
    std::error_code   ec;
    std::size_t       bytes_transferred = 0;
    std::size_t       outstanding = 2;
  };
  class write_op {
    std::shared_ptr<state> state_;
  public:
    using allocator_type = boost::asio::associated_allocator_t<handler_type>;
    auto get_allocator() const noexcept {
      return boost::asio::get_associated_allocator(state_->handler);
    }
    using executor_type = boost::asio::associated_executor_t<handler_type,
                                                             decltype(stream.get_executor())>;
    auto get_executor() const noexcept {
      return boost::asio::get_associated_executor(state_->handler,
                                                  state_->stream.get_executor());
    }
    explicit write_op(std::shared_ptr<state> state) noexcept
      : state_(std::move(state))
    {}
    write_op(write_op&&) = default;
    write_op& operator=(write_op&&) = delete;
    write_op(const write_op&) = delete;
    write_op& operator=(const write_op&) = delete;
    void operator()(std::error_code ec,
                    std::size_t bytes_transferred)
    {
      if (--state_->outstanding) {
        state_->ec = ec;
        state_->bytes_transferred = bytes_transferred;
        state_.reset();
      } else {
        auto h = std::move(state_->handler);
        ec = state_->ec;
        state_.reset();
        h(ec,
          bytes_transferred);
      }
    }
  };
  class timeout_op {
    std::shared_ptr<state> state_;
  public:
    using allocator_type = boost::asio::associated_allocator_t<handler_type>;
    auto get_allocator() const noexcept {
      return boost::asio::get_associated_allocator(state_->handler);
    }
    using executor_type = boost::asio::associated_executor_t<handler_type,
                                                             decltype(timer.get_executor())>;
    auto get_executor() const noexcept {
      return boost::asio::get_associated_executor(state_->handler,
                                                  state_->timer.get_executor());
    }
    explicit timeout_op(std::shared_ptr<state> state) noexcept
      : state_(std::move(state))
    {}
    timeout_op(timeout_op&&) = default;
    timeout_op& operator=(timeout_op&&) = delete;
    timeout_op(const timeout_op&) = delete;
    timeout_op& operator=(const timeout_op&) = delete;
    void operator()(std::error_code) {
      if (--state_->outstanding) {
        state_->ec = make_error_code(std::errc::timed_out);
        state_.reset();
      } else {
        auto h = std::move(state_->handler);
        auto ec = state_->ec;
        auto bytes_transferred = state_->bytes_transferred;
        state_.reset();
        h(ec,
          bytes_transferred);
      }
    }
  };
  completion_type completion(token);
  auto a = boost::asio::get_associated_allocator(completion.completion_handler);
  auto ptr = std::allocate_shared<state>(a,
                                         stream,
                                         timer,
                                         std::move(completion.completion_handler));
  boost::asio::async_write(ptr->stream,
                           cb,
                           write_op(ptr));
  timer.expires_after(timeout);
  timer.async_wait(timeout_op(std::move(ptr)));
  return completion.result.get();
}

}
}
