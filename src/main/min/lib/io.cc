//
// Created by goldolphin on 2022/6/28.
//

#include "io.h"
#include "min/vm/environment.h"
#include "min/vm/native.h"

namespace min::lib::io {

class PrintInt64 : public native::Procedure {
 public:
  Result<void> Call(Environment* env) const override {
    auto stack = env->call_stack();
    auto frame = TRY(stack->CurrentFrame());
    auto v = TRY(stack->PopPrimitive(frame));
    std::cout << v.int64_value << std::endl;
    return {};
  }
};

class PrintFloat64 : public native::Procedure {
 public:
  Result<void> Call(Environment* env) const override {
    auto stack = env->call_stack();
    auto frame = TRY(stack->CurrentFrame());
    auto v = TRY(stack->PopPrimitive(frame));
    std::cout << v.float64_value << std::endl;
    return {};
  }
};

Result<void> LoadLibIo(ModuleTable* module_table) {
  auto module = TRY(module_table->NewModule("io"));

  assembly::Procedure print_int64("print_int64");
  print_int64.SetParams({ ValueType::INT64 });
  TRY(module->DefineProcedure(std::move(print_int64),
                              std::make_unique<PrintInt64>()));

  assembly::Procedure print_float64("print_float64");
  print_float64.SetParams({ ValueType::FLOAT64 });
  TRY(module->DefineProcedure(std::move(print_float64),
                              std::make_unique<PrintFloat64>()));

  return {};
}

}
