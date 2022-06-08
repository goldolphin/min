//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "module.h"
#include "definition.h"
#include <list>

namespace min {

class StructValue {
 public:
  explicit StructValue(ManagedPtr<Struct> type) : type_(type), count_(type->assembly().Count()) {
    fields_ = std::make_unique<Value[]>(count_);
  }

  Result<Value*> GetValue(CountT i) {
    if (i < 0 || i >= count_) {
      return make_error("Index out of bounds: " + to_string(i));
    }
    return &fields_[i];
  }

  [[nodiscard]] ManagedPtr<Struct> type() const {
    return type_;
  }

 private:
  ManagedPtr<Struct> type_;
  CountT count_;
  std::unique_ptr<Value[]> fields_;
};

class Heap {
 public:
  StructValue* New(ManagedPtr<Struct> type) {
    return new StructValue(type);
  }

  StructValue* Singleton(ManagedPtr<Struct> type) {
    auto singleton = type->singleton_;
    if (singleton == nullptr) {
      return type->singleton_ = New(type);
    }
    return singleton;
  }

 private:
  std::list<StructValue*> singletons_;  // 记录 singleton 值，作为 gc root 的一部分
};
}