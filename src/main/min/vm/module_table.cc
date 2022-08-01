//
// Created by goldolphin on 2022/4/11.
//
#include "module_table.h"
#include "string_utils.h"

namespace min {

Result<Module*> ModuleTable::GetModule(const std::string& name) const {
  return TRY(modules_.Get(name));
}

Result<Module*> ModuleTable::NewModule(const std::string& name) {
  auto module = TRY(heap_->NewPermanent<Module>(name));
  TRY(modules_.Put(name, module));
  return module;
}

Result<Module*> ModuleTable::GetOrNewModule(const std::string& name) {
  auto index = modules_.Find(name);
  if (index < 0) {
    return NewModule(name);
  } else {
    return modules_.Get(index);
  }
}

static Result<Module*> GetModule0(const ModuleTable& module_table,
                                            Module* target_module,
                                            const std::string& name) {
  return (name == target_module->name()) ? target_module : TRY(module_table.GetModule(name));
}

static Result<Primitive> ResolveConstant(const ModuleTable& module_table,
                                         Module* target_module,
                                         const assembly::Constant& constant) {
  Primitive value{};
  switch (constant.type) {
    case PrimitiveType::PROC: {
      std::string m, p;
      std::tie(m, p) = TRY(parse_procedure(constant.origin));
      auto module = TRY(GetModule0(module_table, target_module, m));
      auto proc = TRY(module->GetProcedure(p));
      value.proc.value = proc;
      break;
    }
    case PrimitiveType::TYPE: {
      std::string m, t;
      std::tie(m, t) = TRY(parse_type(constant.origin));
      auto module = TRY(GetModule0(module_table, target_module, m));
      auto type = TRY(module->GetStruct(t));
      value.type.value = type;
      break;
    }
    case PrimitiveType::FIELD: {
      std::string m, t, f;
      std::tie(m, t, f) = TRY(parse_field(constant.origin));
      auto module = TRY(GetModule0(module_table, target_module, m));
      auto type = TRY(module->GetStruct(t));
      value.field.value = TRY(type->assembly().Find(f));
      break;
    }
    case PrimitiveType::BYTE: {
      value.byte.value = TRY(parse_number<ByteT>(constant.origin));
      break;
    }
    case PrimitiveType::INT64: {
      value.int64.value = TRY(parse_number<Int64T>(constant.origin));
      break;
    }
    case PrimitiveType::FLOAT64: {
      value.float64.value = TRY(parse_number<Float64T>(constant.origin));
      break;
    }
    default:
      return make_error("Unknown type: " + std::to_string(static_cast<int>(constant.type)));
  }
  return value;
}

Result<void> ModuleTable::LoadModule(const assembly::Module& assembly) {
  if (modules_.Find(assembly.name()) < 0) {
    return make_error("Module already exists: " + min::to_string(assembly.name()));
  }

  auto module = TRY(heap_->NewPermanent<Module>(assembly.name()));

  // 1. Index structs.
  auto struct_count = assembly.StructCount();
  for (CountT i = 0; i < struct_count; ++i) {
    const auto& t = TRY(assembly.GetStruct(i));
    TRY(DefineStruct(module, t));
  }

  // 2. Index procedures.
  auto proc_count = assembly.StructCount();
  for (CountT i = 0; i < proc_count; ++i) {
    const auto& t = TRY(assembly.GetProcedure(i));
    TRY(DefineProcedure(module, t));
  }

  // 3. Resolve constants.
  auto constant_count = assembly.ConstantCount();
  for (CountT i = 0; i < constant_count; ++i) {
    auto&& c = TRY(assembly.GetConstant(i));
    TRY(DefineConstant(module, c));
  }

  return modules_.Put(module->name(), module);
}

Result<void> ModuleTable::DefineStruct(Module* target_module, assembly::Struct s) {
  return target_module->PutStruct(TRY(heap_->NewPermanent<Struct>(target_module, std::move(s))));
}

Result<void> ModuleTable::DefineProcedure(Module* target_module, assembly::Procedure proc) {
  return target_module->PutProcedure(TRY(heap_->NewPermanent<Procedure>(target_module, std::move(proc))));
}

Result<void> ModuleTable::DefineProcedure(Module* target_module, assembly::Procedure proc, NativeProcedure native_impl) {
  return target_module->PutProcedure(TRY(heap_->NewPermanent<Procedure>(target_module, std::move(proc), native_impl)));
}

Result<void> ModuleTable::DefineConstant(Module* target_module, assembly::Constant constant) { // NOLINT(readability-make-member-function-const)
  auto v = TRY(ResolveConstant(*this, target_module, constant));
  return target_module->PutConstant({ std::move(constant), v });
}
}
