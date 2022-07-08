//
// Created by goldolphin on 2022/6/28.
//

#include "min/lib/io.h"
#include "min/vm/native.h"

namespace min::lib::io {

static Result<void> PrintInt64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v = TRY(stack->PopPrimitive(frame));
  std::cout << v.int64.value << std::endl;
  return {};
}


static Result<void> PrintFloat64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v = TRY(stack->PopPrimitive(frame));
  std::cout << v.float64.value << std::endl;
  return {};
}

[[maybe_unused]] struct LibIoInitializer {
  LibIoInitializer() {
    Native::RegisterProcedure("io", "print_int64", {RetType::VOID, { ValueType::INT64 }, PrintInt64});
    Native::RegisterProcedure("io", "print_float64", {RetType::VOID, { ValueType::FLOAT64 }, PrintInt64});
  }
} initializer;

}
