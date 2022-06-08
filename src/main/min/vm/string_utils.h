//
// Created by goldolphin on 2022/5/6.
//

#pragma once

#include "min/common/result.h"
#include "definition.h"
#include <limits>
#include <string>
#include <tuple>
#include <regex>

namespace min {

template <typename T>
Result<T> parse_number(const std::string& s) {
  try {
    auto src = std::stoll(s);
    if (src < std::numeric_limits<T>::min() || src > std::numeric_limits<T>::max()) {
      return make_error("Integer parsing error: " + s);
    }
    return src;
  } catch (...) {
    return make_error("Integer parsing error: " + s);
  }
}

template <>
Result<DoubleT> parse_number(const std::string& s);

Result<std::tuple<std::string, std::string>> parse_procedure(const std::string& s);

Result<std::tuple<std::string, std::string>> parse_type(const std::string& s);

Result<std::tuple<std::string, std::string, std::string>> parse_field(const std::string& s);

}