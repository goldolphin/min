//
// Created by goldolphin on 2022/6/12.
//

#include "assembly_interpreter.h"
#include "min/common/debug.h"

namespace min {

Result<void> AssemblyInterpreterHandler::ModuleBegin(std::string name) {
  auto ret = engine_->module_table()->GetModule(name);
  if (ret.ok()) {
    DEBUG_LOG("## Use existing module: " << name << std::endl);
    current_module_ = ret.value();
  } else {
    DEBUG_LOG("## Define new module: " << name << std::endl);
    current_module_ = TRY(engine_->module_table()->NewModule(name));
  }
  return {};
}

Result<void> AssemblyInterpreterHandler::ModuleEnd() {
  DEBUG_LOG("## Complete module" << std::endl);
  DEBUG_LOG("## Try to run main()" << std::endl);
  return engine_->CallProcedure(current_module_->name() + ".main", {});
}

Result<void> AssemblyInterpreterHandler::StructBegin(std::string name) {
  DEBUG_LOG("## Define new struct: " << name << std::endl);
  current_struct_ = std::make_unique<assembly::Struct>(name);
  return {};
}

Result<void> AssemblyInterpreterHandler::StructField(std::string name, ValueType type) {
  return current_struct_->Put({name, type});
}

Result<void> AssemblyInterpreterHandler::StructEnd() {
  DEBUG_LOG("## Complete struct" << std::endl);
  return engine_->module_table()->DefineStruct(current_module_, std::move(*current_struct_));
}

Result<void> AssemblyInterpreterHandler::ProcBegin(std::string name) {
  DEBUG_LOG("## Define new procedure: " << name << std::endl);
  current_proc_ = std::make_unique<assembly::Procedure>(name);
  return {};
}

Result<void> AssemblyInterpreterHandler::ProcEnd() {
  DEBUG_LOG("## Complete procedure" << std::endl);
  return engine_->module_table()->DefineProcedure(current_module_, std::move(*current_proc_));
}

Result<void> AssemblyInterpreterHandler::Ret(RetType type) {
  current_proc_->ret_type(type);
  return {};
}

Result<void> AssemblyInterpreterHandler::Params(std::vector<ValueType> types) {
  current_proc_->SetParams(std::move(types));
  return {};
}

Result<void> AssemblyInterpreterHandler::Locals(std::vector<ValueType> types) {
  current_proc_->SetLocals(std::move(types));
  return {};
}

Result<void> AssemblyInterpreterHandler::CodeBegin() {
  DEBUG_LOG("## Define new code block" << std::endl);
  current_op_writer_ = std::make_unique<OpWriter>();
  return {};
}

Result<void> AssemblyInterpreterHandler::CodeEnd() {
  DEBUG_LOG("## Complete code block" << std::endl);
  current_proc_->SetByteCodes(TRY(std::move(*current_op_writer_.release()).ToByteCodes(engine_->module_table(), current_module_)));
  return {};
}

Result<void> AssemblyInterpreterHandler::Op(OpCode op) {
  current_op_writer_->WriteOp(op);
  return {};
}

Result<void> AssemblyInterpreterHandler::Op(OpCode op, CountT operand) {
  current_op_writer_->WriteOp(op, operand);
  return {};
}

Result<void> AssemblyInterpreterHandler::Op(OpCode op, assembly::Constant constant) {
  current_op_writer_->WriteOp(op, std::move(constant));
  return {};
}

Result<void> AssemblyInterpreterHandler::Op(OpCode op, std::string label) {
  current_op_writer_->WriteOp(op, std::move(label));
  return {};
}

Result<void> AssemblyInterpreterHandler::Label(std::string name) {
  current_op_writer_->NewLabel(std::move(name));
  return {};
}

}