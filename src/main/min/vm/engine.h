//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "executor.h"
#include "module_table.h"
#include "environment.h"

namespace min {

class Engine {
 private:
  explicit Engine(Options options);

 public:
  Engine(Engine&&) noexcept = delete;
  Engine(const Engine&) = delete;
  Engine& operator=(Engine&&) = delete;
  Engine& operator=(const Engine&) = delete;

 public:
  static Result<std::unique_ptr<Engine>> Create(Options options = Options::Default());

  [[nodiscard]] ModuleTable* module_table() {
    return &module_table_;
  }

  Result<Value> CallProcedure(const std::string& proc_name,
                              const std::vector<Value>& params);
 private:
  Executor executor_;
  CallStack call_stack_;
  Heap heap_;
  ModuleTable module_table_;
  Environment env_;
};

}
