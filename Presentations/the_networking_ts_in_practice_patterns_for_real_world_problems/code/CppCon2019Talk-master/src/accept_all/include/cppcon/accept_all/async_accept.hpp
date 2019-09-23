#pragma once

#include <system_error>
#include <boost/asio/ts/executor.hpp>
#include <cppcon/accept_one/async_accept.hpp>

namespace cppcon::accept_all {

namespace detail {

template<typename Acceptor,
         typename Iterator,
         typename AfterAccept,
         typename Handler>
struct async_accept_loop_op {
  Acceptor& acc_;
  Iterator begin_, end_;
  AfterAccept after_;
  Handler h_;
  using allocator_type = boost::asio::associated_allocator_t<Handler>;
  auto get_allocator() const noexcept {
    return boost::asio::get_associated_allocator(h_);
  }
  using executor_type = boost::asio::associated_executor_t<Handler,
                                                           decltype(acc_.get_executor())>;
  auto get_executor() const noexcept {
    return boost::asio::get_associated_executor(h_,
                                                acc_.get_executor());
  }
  void initiate(Iterator curr) {
    auto&& acc = acc_;
    auto begin = begin_;
    auto end = end_;
    accept_one::async_accept(acc,
                             begin,
                             curr,
                             end,
                             std::move(*this));
  }
  template<typename Stream>
  void operator()(std::error_code ec,
                  Stream s,
                  Iterator curr)
  {
    if (ec) {
      h_(ec);
      return;
    }
    after_(std::move(s));
    initiate(curr);
  }
};

}

template<typename Acceptor,
         typename Iterator,
         typename AfterAccept,
         typename Token>
auto async_accept(Acceptor& acc,
                  Iterator begin,
                  Iterator end,
                  AfterAccept after_accept,
                  Token&& token)
{
  using signature_type = void(std::error_code);
  using completion_type = boost::asio::async_completion<Token,
                                                        signature_type>;
  completion_type completion(token);
  using op_type = detail::async_accept_loop_op<Acceptor,
                                               Iterator,
                                               AfterAccept,
                                               typename completion_type::completion_handler_type>;
  op_type op{acc,
             begin,
             end,
             std::move(after_accept),
             std::move(completion.completion_handler)};
  op.initiate(begin);
  return completion.result.get();
}

}
