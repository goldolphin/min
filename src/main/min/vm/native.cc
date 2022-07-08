//
// Created by goldolphin on 2022/6/1.
//
#include "native.h"
#include <map>

namespace min {

static std::map<std::string, native::Module>* modules() {
  static auto table = new std::map<std::string, native::Module>();
  return table;
}

native::Module* Native::GetModule(const std::string& module_name) {
  auto table = min::modules();
  auto iter = table->find(module_name);
  if (iter == table->end()) {
    iter = table->emplace(module_name, native::Module { .name = module_name }).first;
  }
  return &iter->second;
}

const std::map<std::string, native::Module>& Native::modules() {
  return *min::modules();
}

NativeModuleInitializer::NativeModuleInitializer(const native::Module& module) {
  auto m = Native::GetModule(module.name);
  for (auto&& p : module.procedures) {
    m->procedures.insert_or_assign(p.first, p.second);
  }
}

void NativeModuleInitializer::initialize() {
  // Nothing need to do.
}

}
