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
  explicit ModuleTable(Heap* heap) : heap_(heap) {}

  [[nodiscard]] Result<Module*> GetModule(const std::string& name) const;
  [[nodiscard]] Result<Module*> NewModule(const std::string& name);
  [[nodiscard]] Result<Module*> GetOrNewModule(const std::string& name);
  Result<void> LoadModule(const assembly::Module& assembly);

  [[nodiscard]] Result<void> ResolveAndDefineConstant(Module* target_module, assembly::Constant constant) const;

 private:
  Heap* heap_;
  IndexedList<Module*> modules_;
};

}
