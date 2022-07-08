//
// Created by goldolphin on 2022/6/1.
//
#include "min/common/debug.h"
#include "ms_collector.h"

namespace min {

void MsCollectorMarker::Touch(StructValue* value) {
  if (value != nullptr && value->collector_info()->flag == CollectorInfo::Flag::UNKNOWN) {
    value->collector_info()->flag = CollectorInfo::Flag::REACHABLE;  // 标记为可达
    reachables_.push_back(value);
  }
}

Result<void> MsCollectorMarker::Mark(min::StructValue* value) {
  Touch(value);

  // Trace descendants.
  while (!reachables_.empty()) {
    auto v = reachables_.back();
    reachables_.pop_back();

    // Touch children.
    auto&& s = v->type()->assembly();
    for (int i = 0; i < s.Count(); ++i) {
      if (TRY(s.Get(i)).type == ValueType::REF) {
        Touch(TRY(v->GetValue(i))->reference.value);
      }
    }
  }
  return {};
}

MsCollector::MsCollector(HeapOptions options)
    : options_(std::move(options)),
      allocated_size_(0),
      last_collect_time_(0) {}

static size_t TypeSize(ManagedPtr<Struct> type) {
  return sizeof(StructValue) + type->assembly().Count() * sizeof(Value);
}

static std::chrono::milliseconds Now() {
  return duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
}

Result<void> MsCollector::Collect(CollectorRootScanner* root_scanner) {
  DEBUG_LOG("## Start GC: allocated=" << allocated_size_ << ", capacity=" << options_.capacity << std::endl);
  MsCollectorMarker marker;

  // 标记 GC Roots.
  TRY(root_scanner->Scan(&marker));

  // 清理所有不可达值
  CountT total_count = 0, reachable_count = 0;
  for (auto it = allocated_values_.begin(); it != allocated_values_.end();) {
    auto&& v = *it;
    total_count += 1;
    if (v.collector_info()->flag == CollectorInfo::Flag::REACHABLE) {
      v.collector_info()->flag = CollectorInfo::Flag::UNKNOWN;  // 清除标记，为下次收集做好初始化
      reachable_count += 1;
      ++it;
    } else {
      allocated_size_ -= TypeSize(v.type());
      it = allocated_values_.erase(it);
    }
  }
  last_collect_time_ = Now();

  DEBUG_LOG("## Complete GC: total_count=" << total_count
                                           << ", reachable_count=" << reachable_count
                                           << ", current_allocated=" << allocated_size_
                                           << std::endl);
  return {};
}

Result<StructValue*> MsCollector::New(ManagedPtr<Struct> type, CollectorRootScanner* root_scanner) {
  auto to_allocate = TypeSize(type);
  bool need_collect = false;
  if (allocated_size_ + to_allocate > options_.capacity) {
    need_collect = true;
  } else if (allocated_size_ > options_.collect_threshold * options_.capacity) { // NOLINT(cppcoreguidelines-narrowing-conversions)
    need_collect = Now() - last_collect_time_ >= options_.min_collect_interval;
  }

  if (need_collect) {
    Collect(root_scanner);
  }

  if (allocated_size_ + to_allocate > options_.capacity) {
    return make_error("Out of memory: allocated=" + to_string(allocated_size_)
      + ", to_allocate=" + to_string(to_allocate)
      + ", capacity=" + to_string(options_.capacity));
  }
  allocated_size_ += to_allocate;
  return &allocated_values_.emplace_back(type);
}

}
