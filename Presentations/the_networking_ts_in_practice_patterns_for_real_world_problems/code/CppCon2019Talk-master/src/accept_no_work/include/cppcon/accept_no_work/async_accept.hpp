#pragma once

#include <algorithm>
#include <cassert>
#include <system_error>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/executor.hpp>

namespace cppcon {
namespace accept_no_work {

namespace detail {

template<class Acceptor,
         class Iterator,
         class AfterAccept,
         class Handler>
class async_accept_op {
  static constexpr bool nothrow = std::is_nothrow_copy_constructible_v<Iterator>    &&
                                  std::is_nothrow_move_constructible_v<AfterAccept> &&
                                  std::is_nothrow_move_constructible_v<Handler>;
  Acceptor& acc_;
  Iterator begin_, end_, curr_;
  AfterAccept after_;
  Handler h_;
public:
  using executor_type = boost::asio::associated_executor_t<Handler,
                                                           decltype(std::declval<Acceptor&>().get_executor())>;
  using allocator_type = boost::asio::associated_allocator_t<Handler>;
  auto get_executor() const noexcept {
    return boost::asio::get_associated_executor(h_,
                                                acc_.get_executor());
  }
  auto get_allocator() const noexcept {
    return boost::asio::get_associated_allocator(h_);
  }
  async_accept_op(Acceptor& acc,
                  Iterator begin,
                  Iterator end,
                  AfterAccept after,
                  Handler h) noexcept(nothrow)
    : acc_      (acc),
      begin_    (begin),
      end_      (end),
      curr_     (begin),
      after_    (std::move(after)),
      h_        (std::move(h))
  {
    assert(begin_ != end_);
    assert(curr_ != end_);
  }
  void initiate() {
    assert(curr_ != end_);
    Acceptor& acc = acc_;
    auto&& ctx = curr_->context();
    acc.async_accept(ctx,
                     std::move(*this));
  }
  template<class Stream>
  void operator()(std::error_code ec,
                  Stream s)
  {
    assert(curr_ != end_);
    if (ec) {
      h_(ec);
      return;
    }
    after_(std::move(s));
    ++curr_;
    if (curr_ == end_) {
      curr_ = begin_;
    }
    initiate();
  }
};

}

template<class Acceptor,
         class Iterator,
         class AfterAccept,
         class CompletionToken>
auto async_accept(Acceptor& acc,
                  Iterator begin,
                  Iterator end,
                  AfterAccept after,
                  CompletionToken&& token)
{
  assert(begin != end);
  using signature_type = void(std::error_code);
  using completion_type = boost::asio::async_completion<CompletionToken,
                                                        signature_type>;
  completion_type completion(token);
  using op_type = detail::async_accept_op<Acceptor,
                                          Iterator,
                                          AfterAccept,
                                          typename completion_type::completion_handler_type>;
  op_type o(acc,
            begin,
            end,
            std::move(after),
            std::move(completion.completion_handler));
  o.initiate();
  return completion.result.get();
}

}
}
