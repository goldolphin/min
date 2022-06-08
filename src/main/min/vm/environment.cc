//
// Created by goldolphin on 2022/4/11.
//
#include "environment.h"

namespace min {

Result<ManagedPtr<Module>> Environment::GetModule(const std::string& name) const {
  return TRY(modules_.Get(name)).ptr();
}

Result<ManagedPtr<Module>> Environment::GetModule(const std::string& name) {
  return TRY(modules_.Get(name))->ptr();
}

Result<void> Environment::NewModule(const std::string& name) {
  return modules_.Put(name, Module::Create(name));
}

Result<void> Environment::LoadModule(const assembly::Module& assembly) {
  if (modules_.Find(assembly.name()).ok()) {
    return make_error("Module already exists: " + min::to_string(assembly.name()));
  }
  auto module = TRY(Module::FromAssembly(*this, assembly));
  return modules_.Put(module.ptr()->name(), std::move(module));
}

}