//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "environment.h"

namespace min {

class Executor {
 public:
  Result<void> InvokeOp(Environment *env);
  Result<Value> CallProcedure(Environment *env,
                              const std::string& proc_name,
                              const std::vector<Value>& params);
};

}

