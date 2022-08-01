//
// Created by goldolphin on 2022/4/6.
//

#pragma once

#include <variant>
#include <optional>
#include <string>
#include <utility>
#include <iostream>
#include <list>

namespace min {

class Error {
 public:
  struct Frame {
    const char* function{};
    const char* file{};
    int line{};
  };

  struct Context {
    std::list<Frame> frames;
  };

 public:
  Error() = default;
  Error(Error&&) noexcept = default;
  Error(const Error&) = delete;
  Error& operator=(Error&&) = delete;
  Error& operator=(const Error&) = delete;

  Error(std::string msg, const char* function, const char* file, int line)
      : msg_(std::move(msg)) {
#ifndef NDEBUG
    auto ctx = context();
    ctx->frames.clear();
    ctx->frames.emplace_back(Frame{function, file, line});
#endif
  }

  [[nodiscard]] const std::string& message() const {
    return msg_;
  }

  [[nodiscard]] Context* context() const { // NOLINT(readability-convert-member-functions-to-static)
    thread_local Context ctx;
    return &ctx;
  }

  void Abort() const {
    std::cerr << "Abort: " << message() << "\nRecent backtrace:\n";
    for (auto&& f : context()->frames) {
      std::cerr << "  " << f.function << ", " << f.file << ":" << f.line << '\n';
    }
    std::cerr << std::flush;
    std::abort();
  }

  Error&& AppendFrame(const char* function, const char* file, int line)&& {
#ifndef NDEBUG
    auto ctx = context();
    ctx->frames.emplace_back(Frame{function, file, line});
    return std::move(*this);
#endif
  }

 private:
  std::string msg_;
};

template<class T>
class Result {
 public:
  Result(Result&&) noexcept = default;
  Result(const Result&) = delete;
  Result& operator=(Result&&) = delete;
  Result& operator=(const Result&) = delete;

  Result(T value) : v_(std::move(value)) {} // NOLINT(google-explicit-constructor)
  Result(Error error) : v_(std::move(error)) {} // NOLINT(google-explicit-constructor)

  [[nodiscard]] const T& value() const& {
    return std::get<1>(v_);
  }

  [[nodiscard]] T&& value()&& {
    return std::get<1>(std::move(v_));
  }

  [[nodiscard]] bool ok() const {
    return v_.index() != 0;
  }

  [[nodiscard]] const Error& error() const& {
    return std::get<0>(v_);
  }

  Error&& error()&& {
    return std::get<0>(std::move(v_));
  }

 private:
  std::variant<Error, T> v_;
};

template<class T>
class Result<T&> {
 public:
  Result(Result&&) noexcept = default;
  Result(const Result&) = delete;
  Result& operator=(Result&&) = delete;
  Result& operator=(const Result&) = delete;

  Result(T& value) : v_(&value) {} // NOLINT(google-explicit-constructor)
  Result(Error error) : v_(std::move(error)) {} // NOLINT(google-explicit-constructor)

  [[nodiscard]] T& value() const {
    return *std::get<1>(v_);
  }

  [[nodiscard]] bool ok() const {
    return v_.index() != 0;
  }

  [[nodiscard]] const Error& error() const& {
    return std::get<0>(v_);
  }

  Error&& error()&& {
    return std::get<0>(std::move(v_));
  }

 private:
  std::variant<Error, T*> v_;
};

template<>
class Result<void> {
 public:
  Result(Result&&) noexcept = default;
  Result(const Result&) = delete;
  Result& operator=(Result&&) = delete;
  Result& operator=(const Result&) = delete;

  Result() = default;
  Result(Error error) : v_(std::move(error)) {} // NOLINT(google-explicit-constructor)

  /**
   * 用于将其他任意类型的Result<T> 转换为该类型，忽略其值，仅保留错误与否的信息
   * @tparam R
   * @param r
   */
  template<class R>
  Result(Result<R>&& r) { // NOLINT(google-explicit-constructor)
    if (!r.ok()) {
      v_.emplace(std::move(r).error());
    }
  }

  void value() const {}

  [[nodiscard]] bool ok() const {
    return !v_;
  }

  [[nodiscard]] const Error& error() const& {
    return *v_;
  }

  Error&& error()&& {
    return *std::move(v_);
  }

 private:
  std::optional<Error> v_;
};

#define make_error(msg) Error(std::move(msg), __FUNCTION__, __FILE__, __LINE__)

/**
 * 尝试获取一个 Result 对象的值，如果 Result 包含错误则从调用位置所在函数返回。TRY 要求传入右值，并将使传入对象失效。这是必要的，因为
 * 1. 如果 Result 包含错误，则需要返回 Result::error() 的右值以传递错误。
 * 2. 如果 Result 不包含错误，则值会被取出，此时虽然不是必须销毁原对象，但为了保证语义明确，统一 move 是更为清晰的做法，也能避免一些隐讳的
 * 错误，比如：保存了 Result 的长生命周期的副本，并 TRY，如果在编译期不确定该副本是否会失效有可能带来隐患。
 * 3. 简化实现，这里使用了 g++ 的 statement expr 扩展。该扩展的返回值是 by value 的，如要正确处理左值引用，需要通过重载包装一个
 * 传递指针的 wrapper 对象（比如通过 Result<const T&> 特化）用于返回，以避免拷贝。
 */
#define TRY(r) ({                                                                         \
std::remove_cv_t<std::remove_reference_t<decltype(r)>>&& _r = (r);                        \
if (!_r.ok()) {                                                                           \
  return std::move(_r).error().AppendFrame(__FUNCTION__, __FILE__, __LINE__);             \
}                                                                                         \
std::move(_r);                                                                            \
}).value()

/**
 * 确认一个 Result 对象是否成功，成功返回其值，失败则退出程序
 */
#define ENSURE(r) ({                                                                      \
std::remove_cv_t<std::remove_reference_t<decltype(r)>>&& _r = (r);                        \
if (!_r.ok()) {                                                                           \
  _r.error().Abort();                                                                     \
}                                                                                         \
std::move(_r);                                                                            \
}).value()

}