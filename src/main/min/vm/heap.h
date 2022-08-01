//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "min/vm/memory/ms_collector.h"
#include "min/vm/memory/simple_collector.h"
#include "definition.h"
#include "options.h"
#include <forward_list>
#include <memory>

namespace min {

class ReferenceType : public CollectableType {
 private:
  friend class Heap;
  Collectable* singleton_ {nullptr};
};

template <class T>
class SimpleReferenceType : public ReferenceType {
 public:
  [[nodiscard]] size_t ValueSize() const override {
    return sizeof(T);
  }

  void InitializeValue(Collectable* collectable) override {
    new(collectable->value()) T{};
  }

  void FinalizeValue(Collectable* collectable) override {
    static_cast<T*>(collectable->value())->~T();
  }
};

class InternalValue {
 public:
  ~InternalValue() = default;
};

class Heap {
 public:
  explicit Heap(HeapOptions options, CollectorRootScanner root_scanner)
    : ref_collector_(std::move(options)),
      root_scanner_([this, scanner = std::move(root_scanner)](CollectorMarker* marker) -> Result<void> {
    for (auto s : singletons_) {
      TRY(marker->Mark(s));
    }
    TRY(scanner.Scan(marker));
    return {};
  }) {}

  Result<Collectable*> New(ReferenceType* type) {
    return ref_collector_.New(type, &root_scanner_);
  }

  Result<Collectable*> Singleton(ReferenceType* type) {
    auto singleton = type->singleton_;
    if (singleton == nullptr) {
      return type->singleton_ = TRY(New(type));
    } else {
      return singleton;
    }
  }

  Result<Collectable*> NewInternal(ReferenceType* type) {
    return internal_collector_.New(type, nullptr);
  }

 private:
  MsCollector ref_collector_;
  std::forward_list<Collectable*> singletons_;  // 记录 singleton 值，作为 gc root 的一部分
  CollectorRootScanner root_scanner_;  // 用于获取 gc roots
  std::forward_list<std::unique_ptr<>>() internal_collector_;  // 管理所有 module 的生命周期
};
}
