//
// Created by goldolphin on 2022/4/11.
//
#include "environment.h"
#include "module.h"
#include "string_utils.h"

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
  return constant_pool_.Find(constant);
}

Result<void> Module::DefineConstant(const Environment& env, assembly::Constant constant) {
  if (constant_pool_.Find(constant).ok()) {
    return make_error("Constant already exists: " + min::to_string(constant));
  }
  auto c = TRY(ResolveConstant(env, std::move(constant)));
  return constant_pool_.Put(c.assembly, c);
}

CountT Module::ConstantCount() const {
  return constant_pool_.Count();
}

Result<ManagedPtr<Module>> Module::GetModule(const Environment& env, const std::string& name) const {
  return (name == this->name()) ? managed_ptr() : TRY(env.GetModule(name));
}

Result<Constant> Module::ResolveConstant(const Environment& env, assembly::Constant constant) {
  Constant result{};
  result.assembly = std::move(constant);
  switch (result.assembly.type) {
    case PrimitiveType::PROCEDURE: {
      std::string m, p;
      std::tie(m, p) = TRY(parse_procedure(result.assembly.origin));
      auto module = TRY(GetModule(env, m));
      auto proc = TRY(module->GetProcedure(p));
      result.value.procedure_value = proc.get();
      break;
    }
    case PrimitiveType::TYPE: {
      std::string m, t;
      std::tie(m, t) = TRY(parse_type(result.assembly.origin));
      auto module = TRY(GetModule(env, m));
      auto type = TRY(module->GetStruct(t));
      result.value.type_value = type.get();
      break;
    }
    case PrimitiveType::FIELD: {
      std::string m, t, f;
      std::tie(m, t, f) = TRY(parse_field(result.assembly.origin));
      auto module = TRY(GetModule(env, m));
      auto type = TRY(module->GetStruct(t));
      result.value.field_value = TRY(type->assembly().Find(f));
      break;
    }
    case PrimitiveType::BYTE: {
      result.value.byte_value = TRY(parse_number<ByteT>(result.assembly.origin));
      break;
    }
    case PrimitiveType::INT64: {
      result.value.int64_value = TRY(parse_number<Int64T>(result.assembly.origin));
      break;
    }
    case PrimitiveType::FLOAT64: {
      result.value.float64_value = TRY(parse_number<Float64T>(result.assembly.origin));
      break;
    }
    default:
      return make_error("Unknown type: " + std::to_string(static_cast<int>(result.assembly.type)));
  }
  return result;
}

Result<Managed<Module>> Module::FromAssembly(const Environment& env, const assembly::Module& assembly) {
  auto result = Module::Create(assembly.name());

  // 1. Index structs.
  auto struct_count = assembly.StructCount();
  for (CountT i = 0; i < struct_count; ++i) {
    const auto& t = TRY(assembly.GetStruct(i));
    TRY(result.ptr()->DefineStruct(t));
  }

  // 2. Index procedures.
  auto proc_count = assembly.StructCount();
  for (CountT i = 0; i < proc_count; ++i) {
    const auto& t = TRY(assembly.GetProcedure(i));
    TRY(result.ptr()->DefineProcedure(t));
  }

  // 3. Resolve constants.
  auto constant_count = assembly.ConstantCount();
  for (CountT i = 0; i < constant_count; ++i) {
    auto&& c = TRY(assembly.GetConstant(i));
    TRY(result.ptr()->DefineConstant(env, c));
  }

  return result;
}

}