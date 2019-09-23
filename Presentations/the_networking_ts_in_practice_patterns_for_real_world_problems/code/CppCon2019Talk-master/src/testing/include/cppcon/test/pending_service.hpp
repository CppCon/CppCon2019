#pragma once

#include <cassert>
#include <list>
#include <memory>
#include <mutex>
#include <utility>
#include <boost/asio/ts/executor.hpp>

namespace cppcon {
namespace test {

template<typename Signature>
class pending_service;

template<typename R,
         typename... Args>
class pending_service<R(Args...)> : public boost::asio::execution_context::service {
public:
  using key_type = pending_service;
  explicit pending_service(boost::asio::execution_context& ctx)
    : boost::asio::execution_context::service(ctx),
      shutdown_                              (false)
  {}
  inline static boost::asio::execution_context::id id;
private:
  class storage_base {
  public:
    storage_base() = default;
    storage_base(const storage_base&) = delete;
    storage_base& operator=(const storage_base&) = delete;
    virtual ~storage_base() noexcept {}
    virtual R invoke(Args...) = 0;
  };
  template<typename T>
  class storage : public storage_base {
  public:
    explicit storage(T t) noexcept(std::is_nothrow_move_constructible_v<T>)
      : obj_(std::move(t))
    {}
    virtual R invoke(Args... args) override {
      return obj_(std::forward<Args>(args)...);
    }
  private:
    T obj_;
  };
  class function_type {
  public:
    function_type() = default;
    function_type(const function_type&) = delete;
    function_type& operator=(const function_type&) = delete;
    template<typename T>
    function_type& operator=(T&& t) {
      storage_ = std::make_unique<storage<T>>(std::forward<T>(t));
      return *this;
    }
    explicit operator bool() const noexcept {
      return bool(storage_);
    }
    R operator()(Args... args) {
      assert(storage_);
      class guard {
      public:
        explicit guard(std::unique_ptr<storage_base>& ptr) noexcept
          : ptr_(ptr)
        {
          assert(ptr_);
        }
        guard(const guard&) = delete;
        ~guard() noexcept {
          ptr_.reset();
        }
      private:
        std::unique_ptr<storage_base>& ptr_;
      };
      guard g(storage_);
      return storage_->invoke(std::forward<Args>(args)...);
    }
  private:
    std::unique_ptr<storage_base> storage_;
  };
  using functions_type = std::list<function_type>;
public:
  using pointer = typename functions_type::iterator;
  pointer acquire() {
    std::scoped_lock l(m_);
    assert(!shutdown_);
    fs_.emplace_front();
    return fs_.begin();
  }
  void release(pointer p) noexcept {
    std::scoped_lock l(m_);
    if (shutdown_) {
      return;
    }
    fs_.erase(p);
  }
private:
  virtual void shutdown() noexcept override {
    shutdown_ = true;
    fs_.clear();
  }
  mutable std::mutex m_;
  bool               shutdown_;
  functions_type     fs_;
};

}
}
