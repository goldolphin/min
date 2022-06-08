//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "op.h"
#include "module.h"
#include "min/common/result.h"
#include <map>
#include <vector>

namespace min {

class OpWriter {
 public:
  void WriteOp(OpCode op);
  void WriteOperand(CountT operand);
  Result<std::basic_string<ByteT>> ToByteCodes(ManagedPtr<Module> module, const Environment& env) &&;

  OpWriter&& DefineLabel(const std::string& name) && {
    labels_[name] = byte_codes_.size();
    return std::move(*this);
  }

  OpWriter&& Write_nop() && {
    WriteOp(OpCode::NOP);
    return std::move(*this);
  }

  OpWriter&& Write_popp() && {
    WriteOp(OpCode::POPP);
    return std::move(*this);
  }

  OpWriter&& Write_popr() && {
    WriteOp(OpCode::POPR);
    return std::move(*this);
  }

  OpWriter&& Write_loadc(CountT operand) && {
    WriteOp(OpCode::LOADC);
    WriteOperand(operand);
    return std::move(*this);
  }

  OpWriter&& Write_loadc(const assembly::Constant& constant) && {
    WriteOp(OpCode::LOADC);
    constant_refs_[constant].push_back(byte_codes_.size());
    WriteOperand(0);
    return std::move(*this);
  }

  OpWriter&& Write_loadn() && {
    WriteOp(OpCode::LOADN);
    return std::move(*this);
  }

  OpWriter&& Write_loadp(CountT operand) && {
    WriteOp(OpCode::LOADP);
    WriteOperand(operand);
    return std::move(*this);
  }

  OpWriter&& Write_loadr(CountT operand) && {
    WriteOp(OpCode::LOADR);
    WriteOperand(operand);
    return std::move(*this);
  }

  OpWriter&& Write_storep(CountT operand) && {
    WriteOp(OpCode::STOREP);
    WriteOperand(operand);
    return std::move(*this);
  }

  OpWriter&& Write_storer(CountT operand) && {
    WriteOp(OpCode::STORER);
    WriteOperand(operand);
    return std::move(*this);
  }

  OpWriter&& Write_call() && {
    WriteOp(OpCode::CALL);
    return std::move(*this);
  }

  OpWriter&& Write_ret() && {
    WriteOp(OpCode::RET);
    return std::move(*this);
  }

  OpWriter&& Write_if(CountT operand) && {
    WriteOp(OpCode::IF);
    WriteOperand(operand);
    return std::move(*this);
  }

  OpWriter&& Write_if(const std::string& label) && {
    WriteOp(OpCode::IF);
    label_refs_[label].push_back(byte_codes_.size());
    WriteOperand(0);
    return std::move(*this);
  }

  OpWriter&& Write_goto(CountT operand) && {
    WriteOp(OpCode::GOTO);
    WriteOperand(operand);
    return std::move(*this);
  }

  OpWriter&& Write_goto(const std::string& label) && {
    WriteOp(OpCode::GOTO);
    label_refs_[label].push_back(byte_codes_.size());
    WriteOperand(0);
    return std::move(*this);
  }

  OpWriter&& Write_new() && {
    WriteOp(OpCode::NEW);
    return std::move(*this);
  }

  OpWriter&& Write_singleton() && {
    WriteOp(OpCode::SINGLETON);
    return std::move(*this);
  }

  OpWriter&& Write_typeof() && {
    WriteOp(OpCode::TYPEOF);
    return std::move(*this);
  }

  OpWriter&& Write_getp() && {
    WriteOp(OpCode::GETP);
    return std::move(*this);
  }

  OpWriter&& Write_getr() && {
    WriteOp(OpCode::GETR);
    return std::move(*this);
  }

  OpWriter&& Write_setp() && {
    WriteOp(OpCode::SETP);
    return std::move(*this);
  }

  OpWriter&& Write_setr() && {
    WriteOp(OpCode::SETR);
    return std::move(*this);
  }

  OpWriter&& Write_eq_ref() && {
    WriteOp(OpCode::EQ_REF);
    return std::move(*this);
  }

  OpWriter&& Write_neg_int64() && {
    WriteOp(OpCode::NEG_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_add_int64() && {
    WriteOp(OpCode::ADD_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_sub_int64() && {
    WriteOp(OpCode::SUB_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_mul_int64() && {
    WriteOp(OpCode::MUL_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_div_int64() && {
    WriteOp(OpCode::DIV_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_mod_int64() && {
    WriteOp(OpCode::MOD_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_not_int64() && {
    WriteOp(OpCode::NOT_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_and_int64() && {
    WriteOp(OpCode::AND_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_or_int64() && {
    WriteOp(OpCode::OR_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_xor_int64() && {
    WriteOp(OpCode::XOR_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_shl_int64() && {
    WriteOp(OpCode::SHL_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_shr_int64() && {
    WriteOp(OpCode::SHR_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_eq_int64() && {
    WriteOp(OpCode::EQ_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_ne_int64() && {
    WriteOp(OpCode::NE_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_gt_int64() && {
    WriteOp(OpCode::GT_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_ge_int64() && {
    WriteOp(OpCode::GE_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_lt_int64() && {
    WriteOp(OpCode::LT_INT64);
    return std::move(*this);
  }

  OpWriter&& Write_le_int64() && {
    WriteOp(OpCode::LE_INT64);
    return std::move(*this);
  }


 private:
  std::basic_string<ByteT> byte_codes_;

  // label -> offset
  std::map<std::string, size_t> labels_;

  // label -> [operand_offset1, operand_offset2, ...]
  std::map<std::string, std::vector<size_t>> label_refs_;

  // constant -> [operand_offset1, operand_offset2, ...]
  std::map<assembly::Constant, std::vector<size_t>, assembly::Constant::Less> constant_refs_;
};
}