//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "collector.h"
#include "options.h"
#include <list>
#include <chrono>
#include <deque>

namespace min {
class MsCollectorMarker : public CollectorMarker {
 public:
  Result<void> Mark(StructValue* value) override;

 private:
  void Touch(StructValue* value);

 private:
  std::deque<StructValue*> reachables_;
};

class MsCollector : public Collector {
 public:
  explicit MsCollector(HeapOptions options);
  Result<void> Collect(CollectorRootScanner* root_scanner) override;
  Result<StructValue*> New(ManagedPtr<Struct> type, CollectorRootScanner* root_scanner) override;

 private:
  HeapOptions options_;
  std::size_t allocated_size_;
  std::chrono::milliseconds last_collect_time_;
  std::list<StructValue> allocated_values_;
};
}