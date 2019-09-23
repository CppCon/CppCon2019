#pragma once

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/asio/ts/executor.hpp>

namespace cppcon {
namespace basic {

template<typename State,
         typename... Args>
std::shared_ptr<State> allocate_state(Args&&... args) {
  std::tuple<const std::decay_t<Args>&...> t(args...);
  auto&& handler = std::get<const typename State::completion_handler_type&>(t);
  auto a = boost::asio::get_associated_allocator(handler);
  return std::allocate_shared<State>(a,
                                     std::forward<Args>(args)...);
}

}
}
