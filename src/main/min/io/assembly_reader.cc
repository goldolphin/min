//
// Created by goldolphin on 2022/6/10.
//

#include "assembly_reader.h"
#include "assembly_lexer.h"
#include "min/common/to_string.h"
#include <array>

namespace min {

Result<void> AssemblyReader::Process() {
  return Parse();
}

Result<void> AssemblyReader::ExpectDirective(DirectiveCode expected) {
  auto di = TRY(lexer_.ReadDirective());
  if (di == expected) {
    return {};
  }
  return make_error("Expect " + to_string(expected) + ", but got: " + to_string(di));
}

Result<void> AssemblyReader::Parse() {
  TRY(ExpectDirective(DirectiveCode::MODULE_BEGIN));
  return ParseModule();
}

Result<void> AssemblyReader::ParseModule() {
  auto name = TRY(lexer_.ReadId());
  TRY(handler_->ModuleBegin(std::move(name)));
  while (true) {
    auto di = TRY(lexer_.ReadDirective());
    switch (di) {
      case DirectiveCode::MODULE_END:
        return handler_->ModuleEnd();
      case DirectiveCode::STRUCT_BEGIN:
        TRY(ParseStruct());
        break;
      case DirectiveCode::PROC_BEGIN:
        TRY(ParseProcedure());
        break;
      default:
        return make_error("Unexpected directive: " + to_string(di));
    }
  }
}

Result<void> AssemblyReader::ParseStruct() {
  auto name = TRY(lexer_.ReadId());
  TRY(handler_->StructBegin(std::move(name)));
  while (true) {
    auto t = lexer_.ReadId();
    if (!t.ok()) break;
    auto field_name = std::move(t).value();
    auto field_type = TRY(to_value_type(TRY(lexer_.ReadType())));
    TRY(handler_->StructField(std::move(field_name), field_type));
  }
  TRY(ExpectDirective(DirectiveCode::STRUCT_END));
  return handler_->StructEnd();
}

Result<void> AssemblyReader::ParseProcedure() {
  auto name = TRY(lexer_.ReadId());
  TRY(handler_->ProcBegin(std::move(name)));
  while (true) {
    auto di = TRY(lexer_.ReadDirective());
    switch (di) {
      case DirectiveCode::PROC_END:
        return handler_->ProcEnd();
      case DirectiveCode::RET:
        TRY(ParseRet());
        break;
      case DirectiveCode::PARAMS:
        TRY(ParseParams());
        break;
      case DirectiveCode::LOCALS:
        TRY(ParseLocals());
        break;
      case DirectiveCode::CODE_BEGIN:
        TRY(ParseCode());
        break;
      default:
        return make_error("Unexpected directive: " + to_string(di));
    }
  }
}

Result<void> AssemblyReader::ParseRet() {
  auto type = TRY(to_ret_type(TRY(lexer_.ReadType())));
  return handler_->Ret(type);
}

Result<void> AssemblyReader::ParseParams() {
  std::vector<ValueType> types;
  while (true) {
    auto t = lexer_.ReadType();
    if (!t.ok()) break;
    types.push_back(TRY(to_value_type(t.value())));
  }
  return handler_->Params(std::move(types));
}

Result<void> AssemblyReader::ParseLocals() {
  std::vector<ValueType> types;
  while (true) {
    auto t = lexer_.ReadType();
    if (!t.ok()) break;
    types.push_back(TRY(to_value_type(t.value())));
  }
  return handler_->Locals(std::move(types));
}

Result<void> AssemblyReader::ParseCode() {
  TRY(handler_->CodeBegin());
  while (true) {
    // 尝试读伪指令
    auto t = lexer_.ReadDirective();
    if (t.ok()) {
      if (t.value() == DirectiveCode::LABEL) {
        auto label = TRY(lexer_.ReadId());
        TRY(handler_->Label(std::move(label)));
      } else if (t.value() == DirectiveCode::CODE_END) {
        return handler_->CodeEnd();
      } else {
        return make_error("Unexpected directive: " + to_string(t.value()));
      }
      continue;
    }

    // 读指令
    auto op = TRY(lexer_.ReadOp());
    switch (op) {
      case OpCode::LOADC: {
        // 特殊处理，方便编码者使用常量
        auto type = TRY(to_primitive_type(TRY(lexer_.ReadType())));
        auto origin = TRY(lexer_.ReadString());
        TRY(handler_->Op(op, { type , std::move(origin) }));
        break;
      }
      case OpCode::LOADP: {
        auto operand = TRY(lexer_.ReadOperand());
        TRY(handler_->Op(op, operand));
        break;
      }
      case OpCode::LOADR: {
        auto operand = TRY(lexer_.ReadOperand());
        TRY(handler_->Op(op, operand));
        break;
      }
      case OpCode::STOREP: {
        auto operand = TRY(lexer_.ReadOperand());
        TRY(handler_->Op(op, operand));
        break;
      }
      case OpCode::STORER: {
        auto operand = TRY(lexer_.ReadOperand());
        TRY(handler_->Op(op, operand));
        break;
      }
      case OpCode::IF: {
        // 特殊处理，通过Label避免编码者手动计算偏移
        auto label = TRY(lexer_.ReadId());
        TRY(handler_->Op(op, std::move(label)));
        break;
      }
      case OpCode::GOTO: {
        // 特殊处理，通过Label避免编码者手动计算偏移
        auto label = TRY(lexer_.ReadId());
        TRY(handler_->Op(op, std::move(label)));
        break;
      }
      default:
        TRY(handler_->Op(op));
        break;
    }
  }
}

}