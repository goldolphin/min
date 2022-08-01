//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "min/vm/definition.h"
#include "min/vm/assembly.h"
#include "min/vm/heap.h"
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

class StructValue {
 public:
  explicit StructValue(CountT count) : count_(count) {
    fields_ = std::make_unique<Value[]>(count_);
  }

  Result<Value*> GetValue(CountT i) {
    if (i < 0 || i >= count_) {
      return make_error("Index out of bounds: " + to_string(i));
    }
    return &fields_[i];
  }

 private:
  CountT count_;
  std::unique_ptr<Value[]> fields_;
};

class Struct : public SimpleReferenceType<StructValue> {
 public:
  Struct(Module* module, assembly::Struct assembly)
    : module_(module), assembly_(std::move(assembly)) {}

  [[nodiscard]] Module* module() const {
    return module_;
  }

  [[nodiscard]] const assembly::Struct& assembly() const {
    return assembly_;
  }

  void InitializeValue(Collectable* collectable) const override {
    new(collectable->value()) StructValue(assembly_.Count());
  }

  void TraverseCollectableFields(Collectable* collectable, CollectableVisitor* visitor) const override {
    CountT count = assembly_.Count();
    auto v = static_cast<StructValue*>(collectable->value());
    for (auto i = 0; i < count; ++i) {
      if (ENSURE(assembly_.Get(i)).type == ValueType::REF) {
        visitor->Visit(ENSURE(v->GetValue(i))->reference.value);
      }
    }
  }

 private:
  Module* module_;
  const assembly::Struct assembly_;
};

class Environment;
using NativeProcedure = Result<void> (*)(Environment* env);

class Procedure : public PermanentValue {
 public:
  Procedure(Module* module, assembly::Procedure assembly, NativeProcedure native_impl)
    : module_(module), assembly_(std::move(assembly)), paramp_num_(0), paramr_num_(0), native_impl_(native_impl) {
    for (CountT i = 0; i < assembly_.ParamCount(); ++i) {
      if (assembly_.GetParam(i).value() != ValueType::REF) {
        paramp_num_ += 1;
      } else {
        paramr_num_ += 1;
      }
    }
  }

  Procedure(Module* module, assembly::Procedure assembly)
      : Procedure(module, std::move(assembly), nullptr) {}

  [[nodiscard]] Module* module() const {
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
  Module* module_;
  const assembly::Procedure assembly_;
  CountT paramp_num_;
  CountT paramr_num_;
  NativeProcedure native_impl_;
};

class Module : public PermanentValue {
 public:
  explicit Module(std::string name) : name_(std::move(name)) { }

  [[nodiscard]] const std::string& name() const {
    return name_;
  }

  [[nodiscard]] Result<Struct*> GetStruct(const std::string& name) const;
  Result<void> PutStruct(Struct* s);

  [[nodiscard]] Result<Procedure*> GetProcedure(const std::string& name) const;
  Result<void> PutProcedure(Procedure* proc);

  [[nodiscard]] Result<const Constant&> GetConstant(CountT i) const;
  [[nodiscard]] Result<CountT> FindConstant(const assembly::Constant& constant) const;
  Result<void> PutConstant(Constant c);
  [[nodiscard]] CountT ConstantCount() const;

 private:
  std::string name_;
  IndexedList<Struct*> struct_table_;
  IndexedList<Procedure*> procedures_table_;
  IndexedList<Constant, assembly::Constant, assembly::Constant::Less> constant_pool_;
};

}