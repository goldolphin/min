//
// Created by goldolphin on 2022/6/13.
//

#pragma once

#include "min/common/result.h"
#include <string>
#include <vector>
#include <optional>

namespace min {

template <class Term>
class TermMatcher {
 public:
  struct TermDefine {
    std::string pattern;
    Term term;
  };

 public:
  explicit TermMatcher(std::vector<TermDefine> term_defines);

  template<class Iter>
  std::optional<Term> match(Iter begin, Iter end);

 private:
  struct Trie {
    Trie() : term(nullptr), children(nullptr) {}
    Term* term;
    std::unique_ptr<Trie[]> children;
  };

  static constexpr std::uint8_t INVALID_ID = 0;

 private:
  std::vector<TermDefine> term_defines_;
  std::vector<std::uint8_t> char2id_;
  std::uint8_t id_count_;
  Trie root_;
};

template<class Term>
TermMatcher<Term>::TermMatcher(std::vector<TermDefine> term_defines)
  : term_defines_(std::move(term_defines)), char2id_(256, INVALID_ID) {
  // 建立字符映射 char -> id，压缩存储空间
  std::uint8_t id_count = 0;
  for (auto&& td : term_defines_) {
    for (auto c : td.pattern) {
      auto u = static_cast<std::uint8_t>(c);
      if (u != 0 && char2id_[u] == 0) {
        char2id_[u] = id_count++;
      }
    }
  }
  id_count_ = id_count;

  // 构建 Trie
  for (auto&& td : term_defines_) {
    auto* node = &root_;
    for (auto c : td.pattern) {
      auto id = char2id_[static_cast<std::uint8_t>(c)];
      if (!node->children) {
        node->children = std::make_unique<Trie[]>(id_count_);
      }
      node = &node->children[id];
    }
    node->term = &td.term;
  }
}

template<class Term>
template<class Iter>
std::optional<Term> TermMatcher<Term>::match(Iter begin, Iter end) {
  auto* node = &root_;
  for (Iter it = begin; it != end; ++it) {
    if (!node->children) {
      return {};
    }
    char c = *it;
    auto id = char2id_[static_cast<std::uint8_t>(c)];
    node = &node->children[id];
  }
  if (!node->term) {
    return {};
  }
  return *node->term;
}

}