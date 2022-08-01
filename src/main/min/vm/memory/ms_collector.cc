//
// Created by goldolphin on 2022/6/1.
//
#include "ms_collector.h"
#include "min/common/debug.h"
#include "min/common/to_string.h"

namespace min {

void MsCollectorMarker::Visit(Collectable* collectable) {
  if (collectable != nullptr && *collectable->flag() == Collectable::Flag::UNKNOWN) {
    *collectable->flag() = Collectable::Flag::REACHABLE;  // 标记为可达
    reachables_.push_back(collectable);
  }
}

Result<void> MsCollectorMarker::Mark(Collectable* collectable) {
  Visit(collectable);

  // Trace descendants.
  while (!reachables_.empty()) {
    auto c = reachables_.back();
    reachables_.pop_back();

    // Touch children.
    c->type()->TraverseCollectableFields(c, this);
  }
  return {};
}

MsCollector::MsCollector(HeapOptions options)
    : options_(std::move(options)),
      allocated_size_(0),
      last_collect_time_(0) {}

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
  for (auto before_it = allocated_collectables_.before_begin();; ) {
    auto it = before_it;
    ++it;
    if(it == allocated_collectables_.end()) {
      break;
    }
    auto&& c = *it;
    total_count += 1;
    if (*c->flag() == Collectable::Flag::REACHABLE) {
      *c->flag() = Collectable::Flag::UNKNOWN;  // 清除标记，为下次收集做好初始化
      reachable_count += 1;
      ++before_it;
    } else {
      allocated_size_ -= c->type()->AllocationSize();
      allocated_collectables_.erase_after(before_it);
    }
  }
  last_collect_time_ = Now();

  DEBUG_LOG("## Complete GC: total_count=" << total_count
                                           << ", reachable_count=" << reachable_count
                                           << ", current_allocated=" << allocated_size_
                                           << std::endl);
  return {};
}

Result<Collectable*> MsCollector::New(const CollectableType* type, CollectorRootScanner* root_scanner) {
  auto to_allocate = type->AllocationSize();
  bool need_collect = false;

  // 检查是否需要GC
  if (allocated_size_ + to_allocate > options_.capacity) {
    need_collect = true;
  } else if (allocated_size_ > options_.collect_threshold * options_.capacity) { // NOLINT(cppcoreguidelines-narrowing-conversions)
    need_collect = Now() - last_collect_time_ >= options_.min_collect_interval;
  }

  // 尝试 GC
  if (need_collect) {
    Collect(root_scanner);
  }

  // 空间仍然不足则失败退出
  if (allocated_size_ + to_allocate > options_.capacity) {
    return make_error("Out of memory: allocated=" + to_string(allocated_size_)
      + ", to_allocate=" + to_string(to_allocate)
      + ", capacity=" + to_string(options_.capacity));
  }

  // 分配内存
  allocated_size_ += to_allocate;
  auto mem = std::malloc(type->AllocationSize());
  if (!mem) {
    return make_error("malloc() fails");
  }
  auto collectable = new (mem) Collectable(type);
  allocated_collectables_.emplace_front(collectable, &Delete);
  return collectable;
}

void MsCollector::Delete(Collectable* collectable) {
  collectable->~Collectable();
  std::free(collectable);
}

}
