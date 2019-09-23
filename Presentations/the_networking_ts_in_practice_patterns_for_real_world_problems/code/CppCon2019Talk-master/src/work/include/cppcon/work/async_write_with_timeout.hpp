#pragma once

#include <cstddef>
#include <memory>
#include <system_error>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/executor.hpp>
#include <cppcon/basic/allocate_state.hpp>
#include <cppcon/basic/op_base.hpp>
#include <cppcon/cancelable/async_write_stream_wrapper.hpp>
#include <cppcon/work/state_base.hpp>

namespace cppcon {
namespace work {

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
  using executor_type = decltype(stream.get_executor());
  using timer_executor_type = decltype(timer.get_executor());
  using state_base_type = work::state_base<executor_type,
                                           handler_type,
                                           timer_executor_type>;
  struct state : public state_base_type {
    state(AsyncWriteStream& s,
          Timer& t,
          handler_type h) noexcept(std::is_nothrow_move_constructible_v<handler_type>)
      : state_base_type(s.get_executor(),
                        std::move(h),
                        t.get_executor()),
        stream         (s),
        timer          (t)
    {}
    cancelable::async_write_stream_wrapper<AsyncWriteStream> stream;
    Timer&                                                   timer;
    std::size_t                                              bytes_transferred = 0;
  };
  using op_base_type = basic::op_base<state>;
  struct write_op : public op_base_type {
    using op_base_type::op_base_type;
    void operator()(std::error_code ec,
                    std::size_t bytes_transferred)
    {
      op_base_type::state().timer.cancel();
      if (op_base_type::complete(ec)) {
        op_base_type::upcall(bytes_transferred);
        return;
      }
      op_base_type::state().bytes_transferred = bytes_transferred;
      op_base_type::reset();
    }
  };
  struct timeout_op : public op_base_type {
    using op_base_type::op_base_type;
    void operator()(std::error_code) {
      op_base_type::state().stream.cancel();
      if (op_base_type::complete(make_error_code(std::errc::timed_out))) {
        auto bytes_transferred = op_base_type::state().bytes_transferred;
        op_base_type::upcall(bytes_transferred);
        return;
      }
      op_base_type::reset();
    }
  };
  completion_type completion(token);
  auto ptr = basic::allocate_state<state>(stream,
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
