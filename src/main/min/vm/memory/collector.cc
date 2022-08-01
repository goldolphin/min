//
// Created by goldolphin on 2022/7/29.
//

#include "collector.h"

namespace min {

Collectable::Collectable(const CollectableType* type) : type_(type), flag_(Flag::UNKNOWN) {
  type_->InitializeValue(this);
}

Collectable::~Collectable() {
  type_->FinalizeValue(this);
}

}