//
// Created by goldolphin on 2022/6/28.
//

#include "min/lib/io.h"

namespace min::lib::io {

static Result<void> PrintInt64(min_int64_t v) {
  std::cout << v.value << std::endl;
  return {};
}

static Result<void> PrintFloat64(min_float64_t v) {
  std::cout << v.value << std::endl;
  return {};
}

NativeModuleInitializer initializer (
  {
    .name = "io",
    .procedures = {
      {"print_int64", NATIVE_PROCEDURE(PrintInt64) },
      {"print_float64", NATIVE_PROCEDURE(PrintFloat64) },
    },
  }
);

}
