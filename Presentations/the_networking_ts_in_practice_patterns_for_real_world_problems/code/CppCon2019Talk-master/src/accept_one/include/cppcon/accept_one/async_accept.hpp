#pragma once

#include <cassert>
#include <system_error>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/executor.hpp>
#include <boost/system/error_code.hpp>

namespace cppcon::accept_one::detail {

template<class Acceptor,
         class Iterator,
         class CompletionHandler>
struct async_accept_op {
  Acceptor& acc_;
  Iterator begin_, curr_, end_;
  CompletionHandler h_;
  using allocator_type = boost::asio::associated_allocator_t<CompletionHandler>;
  auto get_allocator() const noexcept {
    return boost::asio::get_associated_allocator(h_);
  }
  using executor_type = boost::asio::associated_executor_t<CompletionHandler,
                                                           decltype(acc_.get_executor())>;
  auto get_executor() const noexcept {
    return boost::asio::get_associated_executor(h_,
                                                acc_.get_executor());
  }
  template<typename Stream>
  void operator()(std::error_code ec,
                  Stream s)
  {
    ++curr_;
    if (curr_ == end_) {
      curr_ = begin_;
    }
    h_(ec,
       std::move(s),
       curr_);
  }
};

class async_accept_token {};

}

namespace boost::asio {

template<typename ErrorCode,
         typename Stream>
class async_result<cppcon::accept_one::detail::async_accept_token,
                   void(ErrorCode,
                        Stream)>
{
public:
  class completion_handler_type {
  public:
    explicit completion_handler_type(cppcon::accept_one::detail::async_accept_token);
    template<typename... Args>
    void operator()(Args&&...);
  };
  using return_type = Stream;
  explicit async_result(completion_handler_type&);
  return_type get();
};

}

namespace cppcon::accept_one {

namespace detail {

template<typename Acceptor>
using accepted_stream_t = decltype(std::declval<Acceptor&>().async_accept(std::declval<Acceptor&>().get_executor().context(),
                                                                          std::declval<async_accept_token>()));

}

template<typename Acceptor,
         typename Iterator,
         typename Token>
auto async_accept(Acceptor& acc,
                  Iterator begin,
                  Iterator curr,
                  Iterator end,
                  Token&& token)
{
  using stream_type = detail::accepted_stream_t<Acceptor>;
  using signature_type = void(std::error_code,
                              stream_type,
                              Iterator);
  using completion_type = boost::asio::async_completion<Token,
                                                        signature_type>;
  completion_type completion(token);
  using op_type = detail::async_accept_op<Acceptor,
                                          Iterator,
                                          typename completion_type::completion_handler_type>;
  op_type op{acc, begin, curr, end, std::move(completion.completion_handler)};
  acc.async_accept(curr->context(), std::move(op));
  return completion.result.get();
}

}
