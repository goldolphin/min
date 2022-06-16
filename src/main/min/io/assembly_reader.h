//
// Created by goldolphin on 2022/6/10.
//

#pragma once

#include "assembly_lexer.h"
#include "min/common/result.h"
#include "min/vm/assembly.h"
#include <iostream>

namespace min {

class AssemblyReaderHandler {
 public:
  virtual Result<void> ModuleBegin(std::string name) = 0;
  virtual Result<void> ModuleEnd() = 0;
  virtual Result<void> StructBegin(std::string name) = 0;
  virtual Result<void> StructField(std::string name, ValueType type) = 0;
  virtual Result<void> StructEnd() = 0;
  virtual Result<void> ProcBegin(std::string name) = 0;
  virtual Result<void> ProcEnd() = 0;
  virtual Result<void> Ret(RetType type) = 0;
  virtual Result<void> Params(std::vector<ValueType> types) = 0;
  virtual Result<void> Locals(std::vector<ValueType> types) = 0;
  virtual Result<void> CodeBegin() = 0;
  virtual Result<void> CodeEnd() = 0;
  virtual Result<void> Op(OpCode op) = 0;
  virtual Result<void> Op(OpCode op, CountT operand) = 0;
  virtual Result<void> Op(OpCode op, assembly::Constant constant) = 0;
  virtual Result<void> Op(OpCode op, std::string label) = 0;
  virtual Result<void> Label(std::string name) = 0;

  ~AssemblyReaderHandler() = default;
};

class AssemblyReader {
 public:
  AssemblyReader(AssemblyReaderHandler* handler, std::istream* stream) : handler_(handler), lexer_(stream) {}
  Result<void> Process();

 private:
  Result<void> ExpectDirective(DirectiveCode expected);
  Result<void> Parse();
  Result<void> ParseModule();
  Result<void> ParseStruct();
  Result<void> ParseProcedure();
  Result<void> ParseRet();
  Result<void> ParseParams();
  Result<void> ParseLocals();
  Result<void> ParseCode();

 private:
  AssemblyReaderHandler* handler_;
  AssemblyLexer lexer_;
};
}