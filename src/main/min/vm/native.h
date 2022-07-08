//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "min/vm/environment.h"

namespace min {

struct NativeProcedureDefinition {
  RetType ret_type;
  std::vector<ValueType> params;
  NativeProcedure proc;
};

struct NativeModuleDefinition {
  std::map<std::string, NativeProcedureDefinition> procedures;
};

namespace {

template <class T>
struct ToValueType {};
#define DEFINE_TYPE(id, code, name) template<> struct ToValueType<min_##name##_t> {static constexpr ValueType type = ValueType::id;};
MIN_VALUE_TYPES
#undef DEFINE_TYPE

template <class T>
struct ToRetType {};
#define DEFINE_TYPE(id, code, name) template<> struct ToRetType<min_##name##_t> {static constexpr RetType type = RetType::id;};
MIN_VALUE_TYPES
#undef DEFINE_TYPE

template<class Func, Func func>
struct NativeProcedureWrapper {
};



template<Result<void>(*func)()>
struct NativeProcedureWrapper<Result<void>(*)(), func> {
  static RetType ret_type() {
    return RetType::VOID;
  }

  static std::vector<ValueType> params() {
    return {};
  }

  static Result<void> native_procedure(Environment* env) {
    return func();
  }
};

template<class R, Result<R>(*func)()>
struct NativeProcedureWrapper<Result<R>(*)(), func> {
  static RetType ret_type() {
    return RetType::VOID;
  }

  static std::vector<ValueType> params() {
    return {};
  }

};

}

class Native {
 public:
  static void RegisterProcedure(const std::string& module_name,
                                const std::string& proc_name,
                                NativeProcedureDefinition definition);
  static const std::map<std::string, NativeModuleDefinition>& native_table();
};

struct NativeProcedureInitializer {
  NativeProcedureInitializer(const std::string& module_name,
                             const std::string& proc_name,
                             NativeProcedureDefinition definition);

};


}
