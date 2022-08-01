//
// Created by goldolphin on 2022/4/11.
//
#include "module.h"

namespace min {

Result<Struct*> Module::GetStruct(const std::string& name) const {
  return TRY(struct_table_.Get(name));
}

Result<void> Module::PutStruct(Struct* s) {
  return struct_table_.Put(s->assembly().name(), s);
}

Result<Procedure*> Module::GetProcedure(const std::string& name) const {
  return TRY(procedures_table_.Get(name));
}

Result<void> Module::PutProcedure(Procedure* proc) {
  return procedures_table_.Put(proc->assembly().name(), proc);
}

Result<const Constant&> Module::GetConstant(CountT i) const {
  return constant_pool_.Get(i);
}

Result<CountT> Module::FindConstant(const assembly::Constant& constant) const {
  auto index = constant_pool_.Find(constant);
  if (index < 0) {
    return make_error("Constant not found: " + to_string(constant));
  }
  return index;
}

Result<void> Module::PutConstant(Constant c) {
  if (constant_pool_.Find(c.assembly) >= 0) {
    return make_error("Constant already exists: " + min::to_string(c.assembly));
  }
  return constant_pool_.Put(c.assembly, std::move(c));
}

CountT Module::ConstantCount() const {
  return constant_pool_.Count();
}

}