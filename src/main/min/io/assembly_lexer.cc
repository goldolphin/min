//
// Created by goldolphin on 2022/6/12.
//

#include "assembly_lexer.h"
#include "min/vm/string_utils.h"

namespace min {

AssemblyLexer::AssemblyLexer(std::istream* stream)
  : stream_(stream),
    op_matcher_({
#define DEFINE_ITEM(id, code, str) { str, OpCode::id },
#include "min/def/op.def"
#undef DEFINE_ITEM
    }),
    directive_matcher_({
#define DEFINE_ITEM(id, code, str) { str, DirectiveCode::id },
#include "min/def/directive.def"
#undef DEFINE_ITEM
    }),
    type_matcher_({
#define DEFINE_TYPE(id, code, str) { str, Type::id },
      MIN_TYPES
#undef DEFINE_TYPE
    }) {}

enum class State {
  INIT,
  SHARP,
  COMMENT,
  TOKEN,
  END,
};

bool AssemblyLexer::PrepareBuffer() {
  if (!buffer_.empty()) {
    return true;
  }
  if (!stream_->good()) {
    return false;
  }

  State state = State::INIT;
  while (state != State::END) {
    auto c = stream_->get();
    if (!stream_->good()) {
      break;
    }
    if (state == State::INIT) {
      if (c == '#') {
        // 需要进一步判断是注释还是伪指令
        state = State::SHARP;
      } else if (std::isspace(c)) {
        // 跳过空格字符
      } else {
        // 非伪指令的Token
        buffer_.push_back(static_cast<char>(c));
        state = State::TOKEN;
      }
    } else if (state == State::SHARP) {
      if (c == '#') {
        // 行注释开始
        state = State::COMMENT;
      } else if (std::isspace(c)) {
        // '#' 视为特殊伪指令
        buffer_.push_back('#');
        state = State::END;
      } else {
        // 可能是伪指令
        buffer_.push_back('#');
        buffer_.push_back(static_cast<char>(c));
        state = State::TOKEN;
      }
    } else if (state == State::COMMENT) {
      if (c == '\r' || c == '\n') {
        // 行注释结束
        state = State::INIT;
      }
    } else {
      // if (state == State::TOKEN)
      if (std::isspace(c)) {
        // Token 结束
        state = State::END;
      } else {
        // 处理 Token
        buffer_.push_back(static_cast<char>(c));
      }
    }
  }
  return !buffer_.empty();
}

Result<void> AssemblyLexer::EnsureBuffer() {
  if (PrepareBuffer()) {
    return {};
  }
  return make_error("Stream not readable");
}

Result<std::string> AssemblyLexer::ReadToken() {
  TRY(EnsureBuffer());
  return std::move(buffer_);
}

Result<OpCode> AssemblyLexer::ReadOp() {
  TRY(EnsureBuffer());
  auto res = op_matcher_.match(buffer_.begin(), buffer_.end());
  if (res) {
    buffer_.clear();
    return res.value();
  } else {
    return make_error("Expect op, but got: " + buffer_);
  }
}

Result<CountT> AssemblyLexer::ReadOperand() {
  TRY(EnsureBuffer());
  auto res = parse_number<CountT>(buffer_);
  if (res.ok()) {
    buffer_.clear();
  }
  return res;
}

Result<DirectiveCode> AssemblyLexer::ReadDirective() {
  TRY(EnsureBuffer());
  auto res = directive_matcher_.match(buffer_.begin(), buffer_.end());
  if (res) {
    buffer_.clear();
    return res.value();
  } else {
    return make_error("Expect directive, but got: " + buffer_);
  }
}

Result<Type> AssemblyLexer::ReadType() {
  TRY(EnsureBuffer());
  auto res = type_matcher_.match(buffer_.begin(), buffer_.end());
  if (res) {
    buffer_.clear();
    return res.value();
  } else {
    return make_error("Expect type, but got: " + buffer_);
  }
}

Result<std::string> AssemblyLexer::ReadId() {
  static std::regex pattern(R"([_a-zA-Z]\w*)");
  TRY(EnsureBuffer());
  if (std::regex_match(buffer_.begin(), buffer_.end(), pattern)) {
    return std::move(buffer_);
  }
  return make_error("Invalid id: " + buffer_);
}
Result<std::string> AssemblyLexer::ReadString() {
  static std::regex pattern(R"("([^\"]|\.)*")");
  TRY(EnsureBuffer());
  if (std::regex_match(buffer_.begin(), buffer_.end(), pattern)) {
    auto str = buffer_.substr(1, buffer_.size()-2);
    buffer_.clear();
    return std::move(str);
  }
  return make_error("Invalid string: " + buffer_);
}

}