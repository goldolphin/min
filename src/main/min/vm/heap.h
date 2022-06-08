//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "ms_collector.h"
#include "module.h"
#include "definition.h"
#include "options.h"
#include <list>
#include <chrono>

namespace min {
class Heap {
 public:
  explicit Heap(HeapOptions options, CollectorRootScanner root_scanner)
    : collector_(std::move(options)),
      root_scanner_([this, scanner = std::move(root_scanner)](CollectorMarker* marker) -> Result<void> {
    for (auto v : singletons_) {
      TRY(marker->Mark(v));
    }
    TRY(scanner.Scan(marker));
    return {};
  }) {}

  Result<StructValue*> New(ManagedPtr<Struct> type) {
    return collector_.New(type, &root_scanner_);
  }

  Result<StructValue*> Singleton(ManagedPtr<Struct> type) {
    auto singleton = type->singleton_;
    if (singleton == nullptr) {
      return type->singleton_ = TRY(New(type));
    } else {
      return singleton;
    }
  }

 private:
  MsCollector collector_;
  std::list<StructValue*> singletons_;  // 记录 singleton 值，作为 gc root 的一部分
  CollectorRootScanner root_scanner_;  // 用于获取 gc roots
};
}
