//
// Created by goldolphin on 2022/4/1.
//

#include "assembly.h"

namespace min::assembly {

Result<const Struct::Field&> Struct::Get(CountT i) const {
  auto aa = fields_.Get(i);
  return fields_.Get(i);
}

Result<CountT> Struct::Find(const std::string& field_name) const {
  auto index = fields_.Find(field_name);
  if (index < 0) {
    return make_error("Field not found: " + field_name);
  }
  return index;
}

Result<void> Struct::Put(Struct::Field field) {
  return fields_.Put(field.name, std::move(field));
}

CountT Struct::Count() const {
  return fields_.Count();
}

Result<const Struct&> Module::GetStruct(CountT i) const {
  return structs_.Get(i);
}

Result<CountT> Module::FindStruct(const std::string& name) const {
  auto index = structs_.Find(name);
  if (index < 0) {
    return make_error("Struct not found: " + name);
  }
  return index;
}

Result<void> Module::PutStruct(Struct s) {
  return structs_.Put(s.name(), std::move(s));
}

CountT Module::StructCount() const {
  return structs_.Count();
}

Result<const Procedure&> Module::GetProcedure(CountT i) const {
  return procedures_.Get(i);
}

Result<CountT> Module::FindProcedure(const std::string& name) const {
  auto index = procedures_.Find(name);
  if (index < 0) {
    return make_error("Procedure not found: " + name);
  }
  return index;
}

Result<void> Module::PutProcedure(Procedure proc) {
  return procedures_.Put(proc.name(), std::move(proc));
}

CountT Module::ProcedureCount() const {
  return procedures_.Count();
}

Result<const Constant&> Module::GetConstant(CountT i) const {
  return constants_.Get(i);
}

Result<CountT> Module::FindConstant(const Constant& constant) const {
  auto index = constants_.Find(constant);
  if (index < 0) {
    return make_error("Constant not found: " + to_string(constant));
  }
  return index;
}

Result<void> Module::PutConstant(Constant constant) {
  return constants_.Put(constant, std::move(constant));
}

CountT Module::ConstantCount() const {
  return constants_.Count();
}

Result<const Module&> Assembly::GetModule(CountT i) const {
  return modules_.Get(i);
}

Result<CountT> Assembly::FindModule(const std::string& name) const {
  auto index = modules_.Find(name);
  if (index < 0) {
    return make_error("Module not found: " + name);
  }
  return index;
}

Result<void> Assembly::PutModule(Module module) {
  return modules_.Put(module.name(), std::move(module));
}

CountT Assembly::ModuleCount() const {
  return modules_.Count();
}

}
