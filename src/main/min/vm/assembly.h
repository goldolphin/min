//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "min/common/indexed_list.h"
#include "min/common/result.h"
#include "definition.h"
#include "op.h"
#include <string>
#include <vector>
#include <map>

namespace min::assembly {

namespace {
template<class T>
class SimpleList {
 public:
  SimpleList() = default;
  SimpleList(const std::vector<T> values) : values_(std::move(values)) {} // NOLINT(google-explicit-constructor)

  [[nodiscard]] Result<T> Get(CountT index) const {
    if (index < 0 || index >= values_.size()) {
      return make_error("Index not found: " + to_string(index));
    }
    return values_[index];
  }

  [[nodiscard]] CountT Count() const {
    return static_cast<CountT>(values_.size());;
  }

 private:
  std::vector<T> values_;
};
}

struct Constant {
  PrimitiveType type;
  std::string origin;

  struct Less {
    constexpr bool operator()(const min::assembly::Constant& lhs, const min::assembly::Constant& rhs) const {
      if (lhs.type != rhs.type) {
        return lhs.type < rhs.type;
      }
      return lhs.origin < rhs.origin;
    }
  };

  [[nodiscard]] std::string to_string() const {
    return "{" + std::to_string(static_cast<int>(type)) + ", " + origin + "}";
  }
};

class Struct {
 public:
  struct Field {
    std::string name;
    ValueType type;
  };

 public:
  explicit Struct(std::string name) : name_(std::move(name)) { }

  [[nodiscard]] const std::string& name() const {
    return name_;
  }

  [[nodiscard]] Result<const Field&> Get(CountT i) const;
  [[nodiscard]] Result<CountT> Find(const std::string& field_name) const;
  Result<void> Put(Field field);
  [[nodiscard]] CountT Count() const;

 private:
  std::string name_;
  IndexedList<Field> fields_;
};

class Procedure {
 public:
  explicit Procedure(std::string name) : name_(std::move(name)), ret_type_(RetType::VOID) {}

  [[nodiscard]] const std::string& name() const {
    return name_;
  }

  void ret_type(RetType ret_type) {
    ret_type_ = ret_type;
  }

  [[nodiscard]] RetType ret_type() const {
    return ret_type_;
  }

  void SetParams(std::vector<ValueType> params) {
    params_ = std::move(params);
  }

  [[nodiscard]] Result<ValueType> GetParam(CountT i) const {
    return params_.Get(i);
  }

  [[nodiscard]] CountT ParamCount() const {
    return params_.Count();
  }

  void SetLocals(std::vector<ValueType> locals) {
    locals_ = std::move(locals);
  }

  [[nodiscard]] Result<ValueType> GetLocal(CountT i) const {
    return locals_.Get(i);
  }

  [[nodiscard]] CountT LocalCount() const {
    return locals_.Count();
  }

  void SetByteCodes(std::basic_string<ByteT> byte_codes) {
    byte_codes_ = std::move(byte_codes);
  }

  [[nodiscard]] const std::basic_string<ByteT>& GetByteCodes() const {
    return byte_codes_;
  }

 private:
  std::string name_;
  RetType ret_type_;
  SimpleList<ValueType> params_;
  SimpleList<ValueType> locals_;
  std::basic_string<ByteT> byte_codes_;
};

class Module {
 public:
  explicit Module(std::string name) : name_(std::move(name)) { }

  [[nodiscard]] const std::string& name() const {
    return name_;
  }

  [[nodiscard]] Result<const Struct&> GetStruct(CountT i) const;
  [[nodiscard]] Result<CountT> FindStruct(const std::string& name) const;
  Result<void> PutStruct(Struct s);
  [[nodiscard]] CountT StructCount() const;

  [[nodiscard]] Result<const Procedure&> GetProcedure(CountT i) const;
  [[nodiscard]] Result<CountT> FindProcedure(const std::string& name) const;
  Result<void> PutProcedure(Procedure proc);
  [[nodiscard]] CountT ProcedureCount() const;

  [[nodiscard]] Result<const Constant&> GetConstant(CountT i) const;
  [[nodiscard]] Result<CountT> FindConstant(const Constant& constant) const;
  Result<void> PutConstant(Constant constant);
  [[nodiscard]] CountT ConstantCount() const;

 private:
  std::string name_;
  IndexedList<Struct> structs_;
  IndexedList<Procedure> procedures_;
  IndexedList<Constant, Constant, Constant::Less> constants_;
};

class Assembly {
 public:
  explicit Assembly(std::string name) : name_(std::move(name)) { }

  [[nodiscard]] const std::string& name() const {
    return name_;
  }

  [[nodiscard]] Result<const Module&> GetModule(CountT i) const;
  [[nodiscard]] Result<CountT> FindModule(const std::string& name) const;
  Result<void> PutModule(Module module);
  [[nodiscard]] CountT ModuleCount() const;

 private:
  std::string name_;
  IndexedList<Module> modules_;
};
}