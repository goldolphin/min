//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "min/vm/definition.h"
#include "min/vm/assembly.h"
#include "min/common/managed.h"
#include <map>
#include <set>
#include <memory>
#include <utility>

namespace min {

struct Constant {
  assembly::Constant assembly;
  Primitive value {};
};

class Module;

class Struct : public EnableManaged<Struct> {
 public:
  Struct(ManagedPtr<Module> module, assembly::Struct assembly)
    : module_(module), assembly_(std::move(assembly)), singleton_(nullptr) {}

  [[nodiscard]] ManagedPtr<Module> module() const {
    return module_;
  }

  [[nodiscard]] const assembly::Struct& assembly() const {
    return assembly_;
  }

 private:
  ManagedPtr<Module> module_;
  const assembly::Struct assembly_;

  // 配合实现 singleton 指令
  friend class Heap;
  StructValue* singleton_;
};

class Environment;
using NativeProcedure = Result<void> (*)(Environment* env);

class Procedure : public EnableManaged<Procedure> {
 public:
  Procedure(ManagedPtr<Module> module, assembly::Procedure assembly, NativeProcedure native_impl)
    : module_(module), assembly_(std::move(assembly)), paramp_num_(0), paramr_num_(0), native_impl_(native_impl) {
    for (CountT i = 0; i < assembly_.ParamCount(); ++i) {
      if (assembly_.GetParam(i).value() != ValueType::REF) {
        paramp_num_ += 1;
      } else {
        paramr_num_ += 1;
      }
    }
  }

  Procedure(ManagedPtr<Module> module, assembly::Procedure assembly)
      : Procedure(module, std::move(assembly), nullptr) {}

  [[nodiscard]] ManagedPtr<Module> module() const {
    return module_;
  }

  [[nodiscard]] const assembly::Procedure& assembly() const {
    return assembly_;
  }

  [[nodiscard]] CountT paramp_num() const {
    return paramp_num_;
  }

  [[nodiscard]] CountT paramr_num() const {
    return paramr_num_;
  }

  [[nodiscard]] NativeProcedure native_impl() const {
    return native_impl_;
  }

 private:
  ManagedPtr<Module> module_;
  const assembly::Procedure assembly_;
  CountT paramp_num_;
  CountT paramr_num_;
  NativeProcedure native_impl_;
};

class Module : public EnableManaged<Module> {
 public:
  explicit Module(std::string name) : name_(std::move(name)) { }

  [[nodiscard]] const std::string& name() const {
    return name_;
  }

  [[nodiscard]] Result<ManagedPtr<Struct>> GetStruct(const std::string& name) const;
  Result<void> DefineStruct(assembly::Struct s);

  [[nodiscard]] Result<ManagedPtr<Procedure>> GetProcedure(const std::string& name) const;
  Result<void> DefineProcedure(assembly::Procedure proc);
  Result<void> DefineProcedure(assembly::Procedure proc, NativeProcedure native_impl);

  [[nodiscard]] Result<const Constant&> GetConstant(CountT i) const;
  [[nodiscard]] Result<CountT> FindConstant(const assembly::Constant& constant) const;
  Result<void> DefineConstant(assembly::Constant constant, Primitive value);
  [[nodiscard]] CountT ConstantCount() const;

 private:
  std::string name_;
  IndexedList<Managed<Struct>> struct_table_;
  IndexedList<Managed<Procedure>> procedures_table_;
  IndexedList<Constant, assembly::Constant, assembly::Constant::Less> constant_pool_;
};

}