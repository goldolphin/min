//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "min/common/result.h"

namespace min {
class Environment;
}

namespace min::native {

class Procedure {
 public:
  virtual Result<void> Call(Environment* env) const = 0;
  virtual ~Procedure() = default;
};

}