//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "stack.h"
#include "heap.h"
#include "module.h"

namespace min {

class Environment {
 public:
  explicit Environment(Options options = Options::Default())
    : heap_(std::move(options.heap_options), call_stack_.CreateRootScanner()) {}

  [[nodiscard]] Result<ManagedPtr<Module>> GetModule(const std::string& name) const;
  [[nodiscard]] Result<ManagedPtr<Module>> GetModule(const std::string& name);
  Result<void> NewModule(const std::string& name);
  Result<void> LoadModule(const assembly::Module& assembly);

  [[nodiscard]] CallStack* call_stack() {
    return &call_stack_;
  }

  [[nodiscard]] Heap* heap() {
    return &heap_;
  }

 private:
  IndexedList<Managed<Module>> modules_;
  CallStack call_stack_;
  Heap heap_;
};

}
