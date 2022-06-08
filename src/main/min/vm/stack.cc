//
// Created by goldolphin on 2022/4/11.
//
#include "environment.h"

namespace min {

Result<ByteT> CallStack::ReadOp(Frame* frame) { // NOLINT(readability-convert-member-functions-to-static)
  auto&& codes = frame->proc->assembly().GetByteCodes();
  CountT begin = frame->pc;
  if (begin < 0 || begin >= codes.size()) {
    return make_error("Invalid offset: " + to_string(begin));
  }
  frame->pc += 1;
  return codes[begin];
}

union Count_Bytes {
  CountT count;
  ByteT bytes[OPERAND_WIDTH_4];
};

Result<CountT> CallStack::ReadOperand(Frame* frame) { // NOLINT(readability-convert-member-functions-to-static)
  auto&& codes = frame->proc->assembly().GetByteCodes();
  CountT begin = frame->pc;
  CountT end = begin + OPERAND_WIDTH_4;
  if (begin < 0 || end >= codes.size()) {
    return make_error("Invalid offset: " + to_string(begin));
  }
  Count_Bytes ret = {};
  ret.bytes[0] = codes[begin];
  ret.bytes[1] = codes[begin+1];
  ret.bytes[2] = codes[begin+2];
  ret.bytes[3] = codes[begin+3];
  frame->pc += OPERAND_WIDTH_4;
  return ret.count;
}

CollectorRootScanner CallStack::CreateRootScanner() {
  return CollectorRootScanner([this](CollectorMarker* marker) -> Result<void> {
    // 标记求值栈
    for (auto v : *ref_stack_.underlying()) {
      TRY(marker->Mark(v));
    }

    // 标记LocalTable
    for (auto&& f : *frames_.underlying()) {
      auto proc = f.proc;
      if (proc.get() != nullptr) {
        auto&& assembly = proc->assembly();
        for (CountT i = 0; i < assembly.LocalCount(); ++i) {
          if (TRY(assembly.GetLocal(i)) == ValueType::REFERENCE) {
            TRY(marker->Mark(f.locals.GetLocal(i).value()->reference));
          }
        }
      }
    }

    return {};
  });
}

}