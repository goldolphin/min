//
// Created by goldolphin on 2022/6/12.
//

#pragma once

#include "term_matcher.h"
#include "min/vm/directive.h"
#include "min/common/result.h"
#include "min/vm/op.h"
#include <istream>
#include <regex>
#include <vector>

namespace min {

class AssemblyLexer {
 public:
  explicit AssemblyLexer(std::istream* stream);

  Result<std::string> ReadToken();

  Result<OpCode> ReadOp();
  Result<CountT> ReadOperand();
  Result<DirectiveCode> ReadDirective();
  Result<Type> ReadType();
  Result<std::string> ReadId();
  Result<std::string> ReadString();

 private:
  bool PrepareBuffer();
  Result<void> EnsureBuffer();

 private:
  std::istream* stream_;
  TermMatcher<OpCode> op_matcher_;
  TermMatcher<DirectiveCode> directive_matcher_;
  TermMatcher<Type> type_matcher_;
  std::string buffer_;
};

}
