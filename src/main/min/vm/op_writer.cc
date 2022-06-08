//
// Created by goldolphin on 2022/6/1.
//
#include "op_writer.h"

namespace min {

union Count_Bytes {
  CountT count;
  ByteT bytes[OPERAND_WIDTH_4];
};

void OpWriter::WriteOp(OpCode op) {
  byte_codes_.push_back(static_cast<ByteT>(op));
}

void OpWriter::WriteOperand(CountT operand) {
  Count_Bytes cb = { .count = operand };
  byte_codes_.push_back(cb.bytes[0]);
  byte_codes_.push_back(cb.bytes[1]);
  byte_codes_.push_back(cb.bytes[2]);
  byte_codes_.push_back(cb.bytes[3]);
}

Result<std::basic_string<ByteT>> OpWriter::ToByteCodes(ManagedPtr<Module> module, const Environment& env)&& {
  // 替换 label
  for (auto&& lr : label_refs_) {
    auto it = labels_.find(lr.first);
    if (it == labels_.end()) {
      return make_error("Label is not defined: " + lr.first);
    }
    Count_Bytes cb = { .count = static_cast<CountT>(it->second) };
    for (auto&& r : lr.second) {
      byte_codes_[r] = cb.bytes[0];
      byte_codes_[r+1] = cb.bytes[1];
      byte_codes_[r+2] = cb.bytes[2];
      byte_codes_[r+3] = cb.bytes[3];
    }
  }

  // 查找/定义并替换 constant
  for (auto&& cr : constant_refs_) {
    auto&& c = cr.first;
    CountT index;
    auto res = module->FindConstant(c);
    if (res.ok()) {
      index = res.value();
    } else {
      index = module->ConstantCount();
      ENSURE(module->DefineConstant(env, c));
    }
    Count_Bytes cb = { .count = index };
    for (auto&& r : cr.second) {
      byte_codes_[r] = cb.bytes[0];
      byte_codes_[r+1] = cb.bytes[1];
      byte_codes_[r+2] = cb.bytes[2];
      byte_codes_[r+3] = cb.bytes[3];
    }
  }

  return std::move(byte_codes_);
}

}