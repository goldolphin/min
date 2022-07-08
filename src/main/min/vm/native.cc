//
// Created by goldolphin on 2022/6/1.
//
#include "native.h"
#include <map>

namespace min {

static std::map<std::string, NativeModuleDefinition>* native_table() {
  static auto table = new std::map<std::string, NativeModuleDefinition>();
  return table;
}

void Native::RegisterProcedure(const std::string& module_name,
                               const std::string& proc_name,
                               NativeProcedureDefinition definition) {
  auto table = min::native_table();
  auto iter = table->find(module_name);
  if (iter == table->end()) {
    iter = table->emplace(module_name, NativeModuleDefinition {}).first;
  }
  iter->second.procedures.insert_or_assign(proc_name, std::move(definition));
}

const std::map<std::string, NativeModuleDefinition>& Native::native_table() {
  return *min::native_table();
}

NativeProcedureInitializer::NativeProcedureInitializer(const std::string& module_name,
                                                       const std::string& proc_name,
                                                       NativeProcedureDefinition definition) {
  Native::RegisterProcedure(module_name, proc_name, std::move(definition));
}
}
