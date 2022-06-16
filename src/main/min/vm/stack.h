//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "definition.h"
#include "module.h"
#include "collector.h"
#include "min/common/to_string.h"
#include "min/common/result.h"
#include <vector>
#include <deque>

namespace min {
namespace {
template<class T>
class SimpleStack {
 public:
  template<class... Args>
  Result<void> Push(Args&&... args) {
    values_.emplace_back(std::forward<Args>(args)...);
    return {};
  }

  Result<T> Pop() {
    if (values_.empty()) {
      return make_error("Empty stack");
    }
    T v = values_.back();
    values_.pop_back();
    return v;
  }

  Result<T*> Top() {
    if (values_.empty()) {
      return make_error("Empty stack");
    }
    return &values_.back();
  }

  CountT Count() {
    return static_cast<CountT>(values_.size());
  }

  Result<void> Remove(CountT begin, CountT end) {
    CountT size = values_.size();
    if (begin < 0 || begin > size) {
      return make_error("Invalid begin: " + to_string(begin));
    }
    if (end < begin || end > size) {
      return make_error("Invalid end: " + to_string(end));
    }
    values_.erase(values_.begin() + begin, values_.begin() + end);
    return {};
  }

  std::deque<T>* underlying() {
    return &values_;
  }

 private:
  std::deque<T> values_;
};
}

class LocalTable {
 public:
  explicit LocalTable(CountT local_num) : values_(local_num) { }

  Result<Value*> GetLocal(CountT index) {
    if (index < 0 || index >= values_.size()) {
      return make_error("Index not found: " + to_string(index));
    }
    return &values_[index];
  }

 private:
  std::vector<Value> values_;
};

struct Frame {
 private:
  friend class CallStack;

 public:
  Frame(CountT local_num,
        CountT primitive_stack_bottom,
        CountT ref_stack_bottom,
        ManagedPtr<Procedure> proc)
      : locals(local_num),
        primitive_stack_bottom(primitive_stack_bottom),
        ref_stack_bottom(ref_stack_bottom),
        proc(proc),
        pc(0) {}

 private:
  // Local variable table
  LocalTable locals;

  // Bottoms of operand stacks
  CountT primitive_stack_bottom;
  CountT ref_stack_bottom;

  // Procedure info
  ManagedPtr<Procedure> proc;

  // PC
  CountT pc;
};

class CallStack {
 public:
  CallStack() {
    // 压入一个初始Frame，简化后续判断逻辑
    ENSURE(frames_.Push(0, 0, 0, ManagedPtr<Procedure>()));
  }

  Result<Frame*> CurrentFrame() {
    return frames_.Top();
  }

  Result<void> PushFrame(ManagedPtr<Procedure> proc) {
    auto frame = TRY(CurrentFrame());

    CountT primitive_stack_bottom = primitive_stack_.Count() - proc->paramp_num();
    if (primitive_stack_bottom < frame->primitive_stack_bottom || primitive_stack_bottom > primitive_stack_.Count()) {
      return make_error("Invalid primitive_stack_bottom: " + to_string(primitive_stack_bottom));
    }

    CountT ref_stack_bottom = ref_stack_.Count() - proc->paramr_num();
    if (ref_stack_bottom < frame->ref_stack_bottom || ref_stack_bottom > ref_stack_.Count()) {
      return make_error("Invalid ref_stack_bottom: " + to_string(ref_stack_bottom));
    }

    return frames_.Push(proc->assembly().LocalCount(),
                        primitive_stack_bottom,
                        ref_stack_bottom,
                        proc);
  }

  Result<void> PopFrame() {
    auto frame = TRY(CurrentFrame());
    switch (frame->proc->assembly().ret_type()) {
      case RetType::VOID:
        break;
      case RetType::REFERENCE:
        ref_stack_.Remove(frame->ref_stack_bottom, ref_stack_.Count() - 1);
        break;
      default:
        primitive_stack_.Remove(frame->primitive_stack_bottom, primitive_stack_.Count() -1);
        break;
    }
    return frames_.Pop();
  }

  Result<Value*> GetLocal(Frame* frame, CountT index) { // NOLINT(readability-convert-member-functions-to-static)
    return frame->locals.GetLocal(index);
  }

  Result<void> PushPrimitive(Frame* frame, Primitive value) {
    return primitive_stack_.Push(value);
  }

  Result<Primitive> PopPrimitive(Frame* frame) {
    if (primitive_stack_.Count() <= frame->primitive_stack_bottom) {
      return make_error("Primitive stack empty.");
    }
    return primitive_stack_.Pop();
  }

  Result<void> PushReference(Frame* frame, RefT value) {
    return ref_stack_.Push(value);
  }

  Result<RefT> PopReference(Frame* frame) {
    if (ref_stack_.Count() <= frame->ref_stack_bottom) {
      return make_error("Reference stack empty.");
    }
    return ref_stack_.Pop();
  }

  [[nodiscard]] ManagedPtr<Procedure> GetProcedure(Frame* frame) { // NOLINT(readability-convert-member-functions-to-static)
    return frame->proc;
  }

  [[nodiscard]] ManagedPtr<Module> GetModule(Frame* frame) { // NOLINT(readability-convert-member-functions-to-static)
    return frame->proc->module();
  }

  [[nodiscard]] CountT pc(Frame* frame) { // NOLINT(readability-convert-member-functions-to-static)
    return frame->pc;
  }

  [[nodiscard]] Result<ByteT> ReadOp(Frame* frame);

  [[nodiscard]] Result<CountT> ReadOperand(Frame* frame);

  Result<void> Goto(Frame* frame, CountT offset) { // NOLINT(readability-convert-member-functions-to-static)
    auto&& codes = frame->proc->assembly().GetByteCodes();
    if (offset < 0 || offset >= codes.size()) {
      return make_error("Invalid offset: " + to_string(offset));
    }
    frame->pc = offset;
    return {};
  }

  CollectorRootScanner CreateRootScanner();

 private:
  SimpleStack<Frame> frames_;
  SimpleStack<Primitive> primitive_stack_;
  SimpleStack<RefT> ref_stack_;
};

}