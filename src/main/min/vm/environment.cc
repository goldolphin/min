//
// Created by goldolphin on 2022/4/11.
//
#include "environment.h"

namespace min {

Environment::Environment(ModuleTable* module_table, Options options)
    : module_table_(module_table),
      heap_(std::move(options.heap_options), call_stack_.CreateRootScanner()) {}

}