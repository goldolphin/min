//
// Created by caofuxiang on 2021/5/20.
//

#include "min/lib/io.h"
#include "engine.h"
#include "native.h"

namespace min {

Engine::Engine(Options options) : env_(&module_table_, std::move(options)) {}

Result<std::unique_ptr<Engine>> Engine::Create(Options options) {
  std::unique_ptr<Engine> engine (new Engine(std::move(options)));
  auto module_table = engine->module_table();
  for (auto&& m : Native::native_table()) {
    auto module = TRY(module_table->GetOrNewModule(m.first));
    for (auto&& p : m.second.procedures) {
      assembly::Procedure assembly(p.first);
      assembly.ret_type(p.second.ret_type);
      assembly.SetParams(p.second.params);
      TRY(module->DefineProcedure(std::move(assembly), p.second.proc));
    }
  }
  return std::move(engine);
}

Result<Value> Engine::CallProcedure(const std::string& proc_name,
                                    const std::vector<Value>& params) {
  return executor_.CallProcedure(&env_, proc_name, params);
}

}
