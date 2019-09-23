#pragma once

#include <cstddef>
#include <memory>
#include <system_error>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/executor.hpp>
#include "allocate_state.hpp"
#include "op_base.hpp"
#include "state_base.hpp"

namespace cppcon {
namespace basic {

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
  struct state : public state_base<executor_type,
                                   handler_type>
  {
    state(AsyncWriteStream& s,
          Timer&,
          handler_type h) noexcept(std::is_nothrow_move_constructible_v<handler_type>)
      : state_base<executor_type,
                   handler_type>(s.get_executor(),
                                 std::move(h)),
        stream         (s)
    {}
    AsyncWriteStream& stream;
    std::size_t       bytes_transferred = 0;
  };
  struct write_op : public op_base<state> {
    using op_base<state>::op_base;
    void operator()(std::error_code ec,
                    std::size_t bytes_transferred)
    {
      if (this->complete(ec)) {
        this->upcall(bytes_transferred);
        return;
      }
      this->state().bytes_transferred = bytes_transferred;
      this->reset();
    }
  };
  struct timeout_op : public op_base<state> {
    using op_base<state>::op_base;
    void operator()(std::error_code) {
      if (this->complete(make_error_code(std::errc::timed_out))) {
        auto bytes_transferred = this->state().bytes_transferred;
        this->upcall(bytes_transferred);
        return;
      }
      this->reset();
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
