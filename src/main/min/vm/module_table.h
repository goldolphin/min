//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "stack.h"
#include "heap.h"
#include "module.h"

namespace min {

class ModuleTable {
 public:
  [[nodiscard]] Result<ManagedPtr<Module>> GetModule(const std::string& name) const;
  [[nodiscard]] Result<ManagedPtr<Module>> NewModule(const std::string& name);
  [[nodiscard]] Result<ManagedPtr<Module>> GetOrNewModule(const std::string& name);
  Result<void> LoadModule(const assembly::Module& assembly);

  [[nodiscard]] Result<void> ResolveAndDefineConstant(ManagedPtr<Module> target_module, assembly::Constant constant) const;

 private:
  IndexedList<Managed<Module>> modules_;
};

}
