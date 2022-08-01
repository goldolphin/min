//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "min/vm/memory/collector.h"
#include "min/vm/options.h"
#include <forward_list>
#include <chrono>
#include <deque>

namespace min {
class MsCollectorMarker : public CollectorMarker, private CollectableVisitor {
 public:
  Result<void> Mark(Collectable* collectable) override;

 private:
  void Visit(Collectable* collectable) override;

 private:
  std::deque<Collectable*> reachables_;
};

class MsCollector : public Collector {
 public:
  explicit MsCollector(HeapOptions options);
  Result<void> Collect(CollectorRootScanner* root_scanner) override;
  Result<Collectable*> New(const CollectableType* type, CollectorRootScanner* root_scanner) override;

 private:
  static void Delete(Collectable* collectable);

 private:
  HeapOptions options_;
  std::size_t allocated_size_;
  std::chrono::milliseconds last_collect_time_;
  using UniquePtr = std::unique_ptr<Collectable, decltype(&Delete)>;
  std::forward_list<UniquePtr> allocated_collectables_;
};
}