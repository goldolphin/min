//
// Created by goldolphin on 2022/5/6.
//

#include "string_utils.h"

namespace min {

template <>
Result<Float64T> parse_number(const std::string& s) {
  try {
    auto src = std::stod(s);
    return src;
  } catch (...) {
    return make_error("Double parsing error: " + s);
  }
}

Result<std::tuple<std::string, std::string>> parse_procedure(const std::string& s) {
  static std::regex pattern(R"(([_a-zA-Z]\w*)\.([_a-zA-Z]\w*))");
  std::smatch result;
  if (std::regex_match(s, result, pattern)) {
    return std::make_tuple(result[1].str(), result[2].str());
  }
  return make_error("Procedure parsing error: " + s);
}

Result<std::tuple<std::string, std::string>> parse_type(const std::string& s) {
  static std::regex pattern(R"(([_a-zA-Z]\w*)\.([_a-zA-Z]\w*))");
  std::smatch result;
  if (std::regex_match(s, result, pattern)) {
    return std::make_tuple(result[1].str(), result[2].str());
  }
  return make_error("Type parsing error: " + s);
}

Result<std::tuple<std::string, std::string, std::string>> parse_field(const std::string& s) {
  static std::regex pattern(R"(([_a-zA-Z]\w*)\.([_a-zA-Z]\w*)\.([_a-zA-Z]\w*))");
  std::smatch result;
  if (std::regex_match(s, result, pattern)) {
    return std::make_tuple(result[1].str(), result[2].str(), result[3].str());
  }
  return make_error("Field parsing error: " + s);
}

}