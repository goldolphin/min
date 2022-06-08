//
// Created by goldolphin on 2022/5/24.
//

#pragma once

#include <memory>

namespace min {

template<class T>
struct ManagedPtr {
 private:
  template<class S> friend class Managed;
  template<class S> friend class EnableManaged;

  explicit ManagedPtr(T* p) : p_(p) {}

 public:
  ManagedPtr() : p_(nullptr) { }

  const T* operator->() const {
    return p_;
  }

  T* operator->() {
    return p_;
  }

  const T* get() const {
    return p_;
  }

 private:
  T* p_;
};

template<class T>
class Managed {
 public:
  template<class S> friend class EnableManaged;

  ManagedPtr<T> ptr() const {
    return ManagedPtr(t_.get());
  }

 private:
  explicit Managed(std::unique_ptr<T> t) : t_(std::move(t)) { }

 private:
  std::unique_ptr<T> t_;
};

template<class T>
class EnableManaged {
 public:
  EnableManaged() : ptr_(nullptr) { }
  EnableManaged(EnableManaged&&) noexcept = delete;
  EnableManaged(const EnableManaged&) = delete;
  EnableManaged& operator=(EnableManaged&&) = delete;
  EnableManaged& operator=(const EnableManaged&) = delete;

  template<class... Args>
  static Managed<T> Create(Args&&... args) {
    auto m = Managed(std::make_unique<T>(std::forward<Args>(args)...));
    auto e = static_cast<EnableManaged*>(m.t_.get());
    e->ptr_ = m.ptr();
    return std::move(m);
  }

  ManagedPtr<T> managed_ptr() const {
    return ptr_;
  }

 private:
  ManagedPtr<T> ptr_;
};

}