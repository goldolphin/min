//
// Created by caofuxiang on 2021/5/20.
//

#include "min/lib/io.h"
#include "engine.h"
#include "native.h"

namespace min {

Engine::Engine(Options options) :
  call_stack_(),
  heap_(std::move(options.heap_options), call_stack_.CreateRootScanner()),
  module_table_(&heap_),
  env_(&call_stack_, &heap_, &module_table_) {}

Result<std::unique_ptr<Engine>> Engine::Create(Options options) {
  min::lib::io::initializer.initialize();
  std::unique_ptr<Engine> engine (new Engine(std::move(options)));
  auto module_table = engine->module_table();
  for (auto&& m : Native::modules()) {
    auto module = TRY(module_table->GetOrNewModule(m.first));
    for (auto&& p : m.second.procedures) {
      assembly::Procedure assembly(p.first);
      assembly.ret_type(p.second.ret_type());
      assembly.SetParams(p.second.params());
      TRY(module_table->DefineProcedure(module, std::move(assembly), p.second.proc()));
    }
  }
  return std::move(engine);
}

Result<Value> Engine::CallProcedure(const std::string& proc_name,
                                    const std::vector<Value>& params) {
  return executor_.CallProcedure(&env_, proc_name, params);
}

}
