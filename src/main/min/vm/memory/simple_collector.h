//
// Created by goldolphin on 2022/7/28.
//

#pragma once

#include "collector.h"
#include <forward_list>
#include <memory>

namespace min {

class SimpleCollectorMarker : public CollectorMarker {
 public:
  Result<void> Mark(Collectable* collectable) override {
    return {};
  }
};

class SimpleCollector : public Collector {
 public:
  Result<void> Collect(CollectorRootScanner* root_scanner) override {
    return {};
  }

  Result<Collectable*> New(const CollectableType* type, CollectorRootScanner* root_scanner) override;

 private:
  static void Delete(Collectable* collectable);

 private:
  using UniquePtr = std::unique_ptr<Collectable, decltype(&Delete)>;
  std::forward_list<UniquePtr> allocated_collectables_;
};

}
