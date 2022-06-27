//
// Created by goldolphin on 2022/4/11.
//
#include "module.h"

namespace min {

Result<ManagedPtr<Struct>> Module::GetStruct(const std::string& name) const {
  return TRY(struct_table_.Get(name)).ptr();
}

Result<void> Module::DefineStruct(assembly::Struct s) {
  auto m = Struct::Create(managed_ptr(), std::move(s));
  return struct_table_.Put(m.ptr()->assembly().name(), std::move(m));
}

Result<ManagedPtr<Procedure>> Module::GetProcedure(const std::string& name) const {
  return TRY(procedures_table_.Get(name)).ptr();
}

Result<void> Module::DefineProcedure(assembly::Procedure proc) {
  auto m = Procedure::Create(managed_ptr(), std::move(proc));
  return procedures_table_.Put(m.ptr()->assembly().name(), std::move(m));
}

Result<void> Module::DefineProcedure(assembly::Procedure proc, std::unique_ptr<native::Procedure> native_impl) {
  auto m = Procedure::Create(managed_ptr(), std::move(proc), std::move(native_impl));
  return procedures_table_.Put(m.ptr()->assembly().name(), std::move(m));
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

Result<void> Module::DefineConstant(assembly::Constant constant, Primitive value) {
  if (constant_pool_.Find(constant) >= 0) {
    return make_error("Constant already exists: " + min::to_string(constant));
  }
  Constant c = {std::move(constant), value};
  return constant_pool_.Put(c.assembly, std::move(c));
}

CountT Module::ConstantCount() const {
  return constant_pool_.Count();
}

}