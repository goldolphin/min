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

class ReferenceValue {
 public:
  virtual Result<Value> GetValue(CountT i) = 0;
  virtual Result<void> SetValue(CountT i, Value value) = 0;
  ~ReferenceValue() = default;
};

class PermanentValue {
 public:
  virtual ~PermanentValue() = default;
};

class ReferenceType : public CollectableType, public PermanentValue {
 private:
  friend class Heap;
  mutable Collectable* singleton_ {nullptr};
};

template <class T>
class SimpleReferenceType : public ReferenceType {
 public:
  [[nodiscard]] size_t ValueSize() const override {
    return sizeof(T);
  }

  void FinalizeValue(Collectable* collectable) const override {
    static_cast<T*>(collectable->value())->~T();
  }
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

  Result<Collectable*> New(const ReferenceType* type) {
    return ref_collector_.New(type, &root_scanner_);
  }

  Result<Collectable*> Singleton(const ReferenceType* type) {
    auto singleton = type->singleton_;
    if (singleton == nullptr) {
      return type->singleton_ = TRY(New(type));
    } else {
      return singleton;
    }
  }

  template<class P, class...Args>
  Result<P*> NewPermanent(Args&&... args) {
    auto p = new P(std::forward<Args>(args)...);
    permanent_collector_.emplace_front(p);
    return p;
  }

 private:
  MsCollector ref_collector_;
  std::forward_list<Collectable*> singletons_;  // 记录 singleton 值，作为 gc root 的一部分
  CollectorRootScanner root_scanner_;  // 用于获取 gc roots
  std::forward_list<std::unique_ptr<PermanentValue>> permanent_collector_;  // 管理所有 PermanentValue 的生命周期
};
}
