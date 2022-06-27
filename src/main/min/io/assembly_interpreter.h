//
// Created by goldolphin on 2022/6/12.
//

#pragma once

#include "assembly_reader.h"
#include "min/vm/engine.h"
#include "min/vm/op_writer.h"

namespace min {

class AssemblyInterpreterHandler : public AssemblyReaderHandler {
 public:
  explicit AssemblyInterpreterHandler(std::unique_ptr<Engine> engine) : engine_(std::move(engine)) {}

  Result<void> ModuleBegin(std::string name) override;
  Result<void> ModuleEnd() override;
  Result<void> StructBegin(std::string name) override;
  Result<void> StructField(std::string name, ValueType type) override;
  Result<void> StructEnd() override;
  Result<void> ProcBegin(std::string name) override;
  Result<void> ProcEnd() override;
  Result<void> Ret(RetType type) override;
  Result<void> Params(std::vector<ValueType> types) override;
  Result<void> Locals(std::vector<ValueType> types) override;
  Result<void> CodeBegin() override;
  Result<void> CodeEnd() override;
  Result<void> Op(OpCode op) override;
  Result<void> Op(OpCode op, CountT operand) override;
  Result<void> Op(OpCode op, assembly::Constant constant) override;
  Result<void> Op(OpCode op, std::string label) override;
  Result<void> Label(std::string name) override;

 private:
  ManagedPtr<Module> current_module_;
  std::unique_ptr<assembly::Struct> current_struct_;
  std::unique_ptr<assembly::Procedure> current_proc_;
  std::unique_ptr<OpWriter> current_op_writer_;
  std::unique_ptr<Engine> engine_;
};

}
