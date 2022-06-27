//
// Created by caofuxiang on 2021/5/20.
//

#include "engine.h"
#include "min/lib/io.h"

namespace min {

Engine::Engine(Options options) : env_(&module_table_, std::move(options)) {}

Result<std::unique_ptr<Engine>> Engine::Create(Options options) {
  std::unique_ptr<Engine> engine (new Engine(std::move(options)));
  TRY(lib::io::LoadLibIo(engine->module_table()));
  return std::move(engine);
}

Result<Value> Engine::CallProcedure(const std::string& proc_name,
                                    const std::vector<Value>& params) {
  return executor_.CallProcedure(&env_, proc_name, params);
}

}
