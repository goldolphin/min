//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include <utility>

#include "min/vm/environment.h"

namespace min {
namespace native {
namespace {

template <class T>
struct TypeHelper {};

#define DEFINE_TYPE(id, code, name)                                                               \
template<>                                                                                        \
struct TypeHelper<min_##name##_t> {                                                               \
static constexpr ValueType value_type = ValueType::id;                                            \
static constexpr RetType ret_type = RetType::id;                                                  \
static Result<min_##name##_t> Pop(CallStack* stack, Frame* frame) {                               \
  return TRY(stack->PopPrimitive(frame)).name;                                                    \
}                                                                                                 \
static Result<void> Push(CallStack* stack, Frame* frame, min_##name##_t value) {                  \
  return stack->PushPrimitive(frame, {.name = value});                                            \
}                                                                                                 \
};
MIN_PRIMITIVE_TYPES
#undef DEFINE_TYPE

template<>
struct TypeHelper<min_ref_t> {
  static constexpr ValueType value_type = ValueType::REF;
  static constexpr RetType ret_type = RetType::REF;
  static Result<min_ref_t> Pop(CallStack* stack, Frame* frame) {
    return TRY(stack->PopReference(frame));
  }
  static Result<void> Push(CallStack* stack, Frame* frame, min_ref_t value) {
    return stack->PushReference(frame, value);
  }
};

template <auto base, class Func>
struct ProcedureWrapper;

template <auto base, class R, class A0, class... As>
struct ProcedureWrapper<base, Result<R>(A0, As...)> {
  template<class... Bs>
  static Result<R> proc(Environment* env, Bs... bs) {
    auto stack = env->call_stack();
    auto frame = TRY(stack->CurrentFrame());
    return ProcedureWrapper<base, Result<R>(As...)>::template proc<Bs..., A0>(env, bs..., TRY(TypeHelper<A0>::Pop(stack, frame)));
  }
};

template <auto base, class R>
struct ProcedureWrapper<base, Result<R>()> {
  template<class... Bs>
  static Result<R> proc(Environment* env, Bs... bs) {
    return base(bs...);
  }
};

template <auto func, class Func>
struct ProcedureHelper;

template<auto func, class R, class... Args>
struct ProcedureHelper<func, Result<R>(*)(Args...)> {
  static constexpr auto ret_type = native::TypeHelper<R>::ret_type;
  static constexpr auto params = { native::TypeHelper<Args>::value_type... };

  static Result<void> proc(Environment* env) {
    auto result = ProcedureWrapper<func, Result<R>(Args...)>::proc(env);
    auto stack = env->call_stack();
    auto frame = TRY(stack->CurrentFrame());
    return TypeHelper<R>::Push(stack, frame, TRY(std::move(result)));
  }
};

template<auto func, class... Args>
struct ProcedureHelper<func, Result<void>(*)(Args...)> {
  static constexpr auto ret_type = RetType::VOID;
  static constexpr auto params = { native::TypeHelper<Args>::value_type... };

  static Result<void> proc(Environment* env) {
    return ProcedureWrapper<func, Result<void>(Args...)>::proc(env);
  }
};

}

class Procedure {
 public:
  constexpr Procedure(RetType ret_type, std::initializer_list<ValueType> params, NativeProcedure proc)
      : ret_type_(ret_type), params_(params), proc_(proc) {}

  template<auto func>
  constexpr static Procedure From() {
    using Helper = native::ProcedureHelper<func, decltype(func)>;
    return { Helper::ret_type, Helper::params, Helper::proc };
  }

  [[nodiscard]] RetType ret_type() const {
    return ret_type_;
  }

  [[nodiscard]] const std::initializer_list<ValueType>& params() const {
    return params_;
  }

  [[nodiscard]] NativeProcedure proc() const {
    return proc_;
  }

 private:
  RetType ret_type_;
  std::initializer_list<ValueType> params_;
  NativeProcedure proc_;
};

struct Module {
  std::string name;
  std::map<std::string, Procedure> procedures;
};
}

class Native {
 public:
  static native::Module* GetModule(const std::string& module_name);
  static const std::map<std::string, native::Module>& modules();
};

struct NativeModuleInitializer {
  explicit NativeModuleInitializer(const native::Module& module);
  void initialize();
};

#define NATIVE_PROCEDURE(func) native::Procedure::From<func>()

}