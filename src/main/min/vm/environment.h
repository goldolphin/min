//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "stack.h"
#include "heap.h"
#include "module_table.h"

namespace min {

class Environment {
 public:
  Environment(ModuleTable* module_table, Options options);

  [[nodiscard]] ModuleTable* module_table() {
    return module_table_;
  }

  [[nodiscard]] CallStack* call_stack() {
    return &call_stack_;
  }

  [[nodiscard]] Heap* heap() {
    return &heap_;
  }

 private:
  ModuleTable* module_table_;
  CallStack call_stack_;
  Heap heap_;
};

}
