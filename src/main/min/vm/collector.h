//
// Created by goldolphin on 2022/6/10.
//
#pragma once

#include "module.h"
#include <functional>

namespace min {

struct CollectorInfo {
  enum class Flag : ByteT {
    UNKNOWN = 0,
    REACHABLE = 1,
  };

  CollectorInfo() : flag(Flag::UNKNOWN) {}

  Flag flag;
};

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

  CollectorInfo* collector_info() {
    return &collector_info_;
  }

 private:
  ManagedPtr<Struct> type_;
  CountT count_;
  CollectorInfo collector_info_;
  std::unique_ptr<Value[]> fields_;
};

class CollectorMarker {
 public:
  virtual Result<void> Mark(StructValue* value) = 0;
  virtual ~CollectorMarker() = default;
};

class CollectorRootScanner {
 public:
  explicit CollectorRootScanner(std::function<Result<void>(CollectorMarker*)> impl) : impl_(std::move(impl)) {}

  Result<void> Scan(CollectorMarker* marker) const {
    return impl_(marker);
  }

 private:
  std::function<Result<void>(CollectorMarker*)> impl_;
};

class Collector {
 public:
  virtual Result<void> Collect(CollectorRootScanner* root_scanner) = 0;
  virtual Result<StructValue*> New(ManagedPtr<Struct> type, CollectorRootScanner* root_scanner) = 0;
  virtual ~Collector() = default;
};
}