//
// Created by goldolphin on 2022/5/24.
//

#pragma once

#include "min/common/result.h"
#include "min/common/to_string.h"
#include <map>
#include <vector>

namespace min {
typedef std::int32_t CountT;

template <class T, class Key = std::string, class Compare = std::less<Key>>
class IndexedList {
 public:
  [[nodiscard]] Result<const T&> Get(CountT index) const;
  [[nodiscard]] Result<T*> Get(CountT index);
  [[nodiscard]] Result<const T&> Get(const Key& key) const;
  [[nodiscard]] Result<T*> Get(const Key& key);
  [[nodiscard]] CountT Find(const Key& key) const;
  template <class... Args>
  Result<void> Put(const Key& key, Args&&... args);
  [[nodiscard]] CountT Count() const;

 private:
  std::vector<T> list_;
  std::map<Key, CountT, Compare> index_;
};

template<class T, class Key, class Compare>
Result<const T&> IndexedList<T, Key, Compare>::Get(CountT index) const {
  if (index < 0 || index >= list_.size()) {
    return make_error("Index not found: " + to_string(index));
  }
  return list_[index];
}

template<class T, class Key, class Compare>
Result<T*> IndexedList<T, Key, Compare>::Get(CountT index) {
  if (index < 0 || index >= list_.size()) {
    return make_error("Index not found: " + to_string(index));
  }
  return &(list_[index]);
}

template<class T, class Key, class Compare>
Result<const T&> IndexedList<T, Key, Compare>::Get(const Key& key) const {
  auto index = Find(key);
  if (index < 0) {
    return make_error("Key not found: " + to_string(key));
  }
  return Get(index);
}

template<class T, class Key, class Compare>
Result<T*> IndexedList<T, Key, Compare>::Get(const Key& key) {
  auto index = Find(key);
  if (index < 0) {
    return make_error("Key not found: " + to_string(key));
  }
  return Get(index);
}

template<class T, class Key, class Compare>
CountT IndexedList<T, Key, Compare>::Find(const Key& key) const {
  auto it = index_.find(key);
  if (it == index_.end()) {
    return -1;
  }
  return it->second;
}

template<class T, class Key, class Compare>
template<class... Args>
Result<void> IndexedList<T, Key, Compare>::Put(const Key& key,  Args&&... args) {
  auto it = index_.find(key);
  if (it != index_.end()) {
    return make_error("Key already exists: " + to_string(key));
  }
  index_.emplace(key, Count());
  list_.emplace_back(std::forward<Args>(args)...);
  return {};
}

template<class T, class Key, class Compare>
CountT IndexedList<T, Key, Compare>::Count() const {
  return static_cast<CountT>(list_.size());;
}

}