//
// Created by goldolphin on 2022/6/10.
//
#pragma once

#include "min/vm/definition.h"
#include <functional>
#include <memory>

namespace min {

class CollectableType;

class Collectable final {
 public:
  enum class Flag : ByteT {
    UNKNOWN = 0,
    REACHABLE = 1,
  };

  explicit Collectable(const CollectableType* type);

  ~Collectable();

  const CollectableType* type() {
    return type_;
  }

  Flag* flag() {
    return &flag_;
  }

  void* value() {
    return this + sizeof(Collectable);
  }

 private:
  const CollectableType* type_;
  Flag flag_;
};

class CollectableVisitor {
 public:
  virtual void Visit(Collectable* collectable) = 0;
  virtual ~CollectableVisitor() = default;
};

class CollectableType {
 public:
  [[nodiscard]] std::size_t AllocationSize() const {
    return sizeof(Collectable) + ValueSize();
  }

  [[nodiscard]] virtual std::size_t ValueSize() const = 0;
  virtual void InitializeValue(Collectable* collectable) const = 0;
  virtual void FinalizeValue(Collectable* collectable) const = 0;
  virtual void TraverseCollectableFields(Collectable* collectable, CollectableVisitor* visitor) const = 0;
  virtual ~CollectableType() = default;
};

class CollectorMarker {
 public:
  virtual Result<void> Mark(Collectable* collectable) = 0;
  virtual ~CollectorMarker() = default;
};

class CollectorRootScanner final {
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
  virtual Result<Collectable*> New(const CollectableType* type, CollectorRootScanner* root_scanner) = 0;
  virtual ~Collector() = default;
};
}