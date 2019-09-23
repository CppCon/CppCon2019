#pragma once

#include <cstddef>
#include <utility>
#include <boost/asio/ts/executor.hpp>
#include <boost/system/error_code.hpp>

namespace cppcon {
namespace cancelable_no_bases {

template<typename AsyncWriteStream>
class sticky_cancel_async_write_stream {
  AsyncWriteStream& stream_;
  bool              canceled_ = false;
public:
  explicit sticky_cancel_async_write_stream(AsyncWriteStream& stream) noexcept
    : stream_(stream)
  {}
  auto get_executor() const noexcept {
    return stream_.get_executor();
  }
  auto cancel() {
    canceled_ = true;
    return stream_.cancel();
  }
  template<typename ConstBufferSequence,
           typename Token>
  auto async_write_some(ConstBufferSequence cb,
                        Token&& token)
  {
    if (!canceled_) {
      return stream_.async_write_some(std::move(cb),
                                      std::forward<Token>(token));
    }
    using signature_type = void(boost::system::error_code,
                                std::size_t);
    boost::asio::async_completion<Token,
                                  signature_type> completion(token);
    auto ex = boost::asio::get_associated_executor(completion.completion_handler,
                                                   get_executor());
    auto a = boost::asio::get_associated_allocator(completion.completion_handler);
    auto f = [handler = std::move(completion.completion_handler)]() mutable {
      handler(make_error_code(boost::system::errc::operation_canceled),
              0);
    };
    ex.post(std::move(f),
            a);
    return completion.result.get();
  }
};

}
}
