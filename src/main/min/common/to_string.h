//
// Created by goldolphin on 2022/5/24.
//

#pragma once

#include <string>
#include <utility>
#include <unordered_map>
#include <map>
#include <vector>

namespace min {

/**
 * 支持指针类型的 to_string 重载
 * @tparam T
 * @param t
 * @return
 */
template<class T>
auto to_string(T t) -> decltype(to_string(*t)) {
  return to_string(*t);
}

/**
 * 封装 std::to_string() 的 to_string 重载
 * @tparam T
 * @param t
 * @return
 */
template<class T>
auto to_string(T t) -> decltype(std::to_string(t)) {
  return std::to_string(t);
}

/**
 * 支持实现了 to_string() 方法的类型的 to_string 重载
 * @tparam T
 * @param t
 * @return
 */
template<class T>
auto to_string(T t) -> decltype(t.to_string()) {
  return t.to_string();
}

/**
 * 支持 std::string 类型的 to_string 重载
 * @param str
 * @return
 */
static inline const std::string& to_string(const std::string& str) {
  return str;
}

/**
 * 支持 std::pair 类型的 to_string 重载
  * @tparam T1
  * @tparam T2
  * @param pair
  * @return
  */
template<class T1, class T2>
std::string to_string(const std::pair<T1, T2>& pair) {
  std::string buffer;
  buffer.append(to_string(pair.first)).append(": ").append(to_string(pair.second));
  return buffer;
}

/**
 * 支持 std::vector 类型的 to_string 重载
 * @tparam E
 * @param container
 * @return
 */
template<class E>
std::string to_string(const std::vector<E>& container) {
  std::string buffer;
  buffer.append("[");
  bool first = true;
  for (auto&& e : container) {
    if (first) {
      first = false;
    } else {
      buffer.append(", ");
    }
    buffer.append(to_string(e));
  }
  return buffer.append("]");
}

/**
 * 支持 std::unordered_map 类型的 to_string 重载
 * @tparam K
 * @tparam V
 * @param container
 * @return
 */
template<class K, class V>
std::string to_string(const std::unordered_map<K, V>& container) {
  std::string buffer;
  buffer.append("{");
  bool first = true;
  for (auto&& e : container) {
    if (first) {
      first = false;
    } else {
      buffer.append(", ");
    }
    buffer.append(to_string(e.first)).append(": ").append(to_string(e.second));
  }
  return buffer.append("}");
}

/**
 * 支持 std::map 类型的 to_string 重载
 * @tparam K
 * @tparam V
 * @param container
 * @return
 */
template<class K, class V>
std::string to_string(const std::map<K, V>& container) {
  std::string buffer;
  buffer.append("{");
  bool first = true;
  for (auto&& e : container) {
    if (first) {
      first = false;
    } else {
      buffer.append(", ");
    }
    buffer.append(to_string(e.first)).append(": ").append(to_string(e.second));
  }
  return buffer.append("}");
}

}