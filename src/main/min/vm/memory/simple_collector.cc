//
// Created by goldolphin on 2022/7/28.
//

#include "simple_collector.h"
#include <cstdlib>

namespace min {
Result<Collectable*> SimpleCollector::New(const CollectableType* type, CollectorRootScanner* root_scanner) {
  auto mem = std::malloc(type->AllocationSize());
  if (!mem) {
    return make_error("malloc() fails");
  }
  auto collectable = new (mem) Collectable(type);
  allocated_collectables_.emplace_front(collectable, &Delete);
  return collectable;
}

void SimpleCollector::Delete(Collectable* collectable) {
  collectable->~Collectable();
  std::free(collectable);
}

}