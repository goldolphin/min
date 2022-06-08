//
// Created by caofuxiang on 2021/5/20.
//

#include "engine.h"
#include "string_utils.h"
#include "min/common/debug.h"

/**
 * 过程调用约定采用 stdcall ：参数从右至左依次入栈，出栈顺序为从左至右，由被调用者清理栈空间。
 * 多参数指令调用方式同上。
 */

namespace min {

static Result<void> invoke_nop(Environment* env) {
  return {};
}

static Result<void> invoke_popp(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  return stack->PopPrimitive(frame);
}

static Result<void> invoke_popr(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  return stack->PopReference(frame);
}

static Result<void> invoke_loadc(Environment* env, CountT operand) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v = TRY(stack->GetModule(frame)->GetConstant(operand)).value;
  return stack->PushPrimitive(frame, v);
}

static Result<void> invoke_loadn(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  return stack->PushReference(frame, nullptr);
}

static Result<void> invoke_loadp(Environment* env, CountT operand) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v  = TRY(stack->GetLocal(frame, operand));
  return stack->PushPrimitive(frame, v->primitive);
}

static Result<void> invoke_loadr(Environment* env, CountT operand) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v  = TRY(stack->GetLocal(frame, operand));
  return stack->PushReference(frame, v->reference);
}

static Result<void> invoke_storep(Environment* env, CountT operand) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v = TRY(stack->PopPrimitive(frame));
  TRY(stack->GetLocal(frame, operand))->primitive = v;
  return {};
}

static Result<void> invoke_storer(Environment* env, CountT operand) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v = TRY(stack->PopReference(frame));
  TRY(stack->GetLocal(frame, operand))->reference = v;
  return {};
}

/**
 * 过程调用约定采用 stdcall ：参数从右至左依次入栈，由被调用者清理栈空间
 * @param env
 * @param op
 * @return
 */
static Result<void> invoke_call(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v = TRY(stack->PopPrimitive(frame));
  if (v.procedure_value->native_impl() == nullptr) {
    DEBUG_LOG("## Enter procedure: " << v.procedure_value->assembly().name() << "()" << std::endl);
    return stack->PushFrame(v.procedure_value->managed_ptr());
  } else {
    DEBUG_LOG("## Enter native procedure: " << v.procedure_value->assembly().name() << "()" << std::endl);
    TRY(stack->PushFrame(v.procedure_value->managed_ptr()));
    v.procedure_value->native_impl()->Call(env);
    DEBUG_LOG("## Exit native procedure: " << v.procedure_value->assembly().name() << "()" << std::endl);
    return stack->PopFrame();
  }
}

static Result<void> invoke_ret(Environment* env) {
  auto stack = env->call_stack();
  DEBUG_LOG("## Exit procedure: " << stack->GetProcedure(TRY(stack->CurrentFrame()))->assembly().name() << "()" << std::endl);
  return stack->PopFrame();
}

static Result<void> invoke_if(Environment* env, CountT operand) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto v = TRY(stack->PopPrimitive(frame));
  if (v.byte_value != 0) {
    return stack->Goto(frame, operand);
  }
  return {};
}

static Result<void> invoke_goto(Environment* env, CountT operand) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  return stack->Goto(frame, operand);
}

static Result<void> invoke_new(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto t = TRY(stack->PopPrimitive(frame));
  auto heap = env->heap();
  auto r = TRY(heap->New(t.type_value->managed_ptr()));
  return stack->PushReference(frame, r);
}

static Result<void> invoke_singleton(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto t = TRY(stack->PopPrimitive(frame));
  auto heap = env->heap();
  auto r = TRY(heap->Singleton(t.type_value->managed_ptr()));
  return stack->PushReference(frame, r);
}

static Result<void> invoke_typeof(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto r = TRY(stack->PopReference(frame));
  auto t = r->type();
  return stack->PushPrimitive(frame, { .type_value = t.get() });
}

static Result<void> invoke_getp(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto r = TRY(stack->PopReference(frame));
  auto f = TRY(stack->PopPrimitive(frame));
  auto v = TRY(r->GetValue(f.field_value));
  return stack->PushPrimitive(frame, v->primitive);
}

static Result<void> invoke_getr(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto r = TRY(stack->PopReference(frame));
  auto f = TRY(stack->PopPrimitive(frame));
  auto v = TRY(r->GetValue(f.field_value));
  return stack->PushReference(frame, v->reference);
}

static Result<void> invoke_setp(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto r = TRY(stack->PopReference(frame));
  auto f = TRY(stack->PopPrimitive(frame));
  auto v = TRY(r->GetValue(f.field_value));
  v->primitive = TRY(stack->PopPrimitive(frame));
  return {};
}

static Result<void> invoke_setr(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto r = TRY(stack->PopReference(frame));
  auto f = TRY(stack->PopPrimitive(frame));
  auto v = TRY(r->GetValue(f.field_value));
  v->reference = TRY(stack->PopReference(frame));
  return {};
}

static Result<void> invoke_eq_ref(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopReference(frame));
  auto b = TRY(stack->PopReference(frame));
  return stack->PushPrimitive(frame, {.byte_value = a == b});
}

static Result<void> invoke_neg_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  a.int64_value = -a.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_add_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value += b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_sub_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value -= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_mul_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value *= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_div_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value /= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_mod_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value %= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_not_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  a.int64_value = ~a.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_and_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value &= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_or_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value |= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_xor_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value ^= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_shl_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value <<= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_shr_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  a.int64_value >>= b.int64_value;
  return stack->PushPrimitive(frame, a);
}

static Result<void> invoke_eq_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  return stack->PushPrimitive(frame, {.byte_value = a.int64_value == b.int64_value});
}

static Result<void> invoke_ne_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  return stack->PushPrimitive(frame, {.byte_value = a.int64_value != b.int64_value});
}

static Result<void> invoke_gt_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  return stack->PushPrimitive(frame, {.byte_value = a.int64_value > b.int64_value});
}

static Result<void> invoke_ge_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  return stack->PushPrimitive(frame, {.byte_value = a.int64_value >= b.int64_value});
}

static Result<void> invoke_lt_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  return stack->PushPrimitive(frame, {.byte_value = a.int64_value <= b.int64_value});
}

static Result<void> invoke_le_int64(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto a = TRY(stack->PopPrimitive(frame));
  auto b = TRY(stack->PopPrimitive(frame));
  return stack->PushPrimitive(frame, {.byte_value = a.int64_value <= b.int64_value});
}

Result<void> Engine::InvokeOp(Environment* env) {
  auto stack = env->call_stack();
  auto frame = TRY(stack->CurrentFrame());
  auto pc = stack->pc(frame);
  OpCode op = static_cast<OpCode>(TRY(stack->ReadOp(frame)));
  switch (op) {
    case OpCode::NOP:
      DEBUG_LOG(pc << ": nop" << std::endl);
      return invoke_nop(env);
    case OpCode::POPP:
      DEBUG_LOG(pc << ": popp" << std::endl);
      return invoke_popp(env);
    case OpCode::POPR:
      DEBUG_LOG(pc << ": popr" << std::endl);
      return invoke_popr(env);
    case OpCode::LOADC: {
      auto operand = TRY(stack->ReadOperand(frame));
      DEBUG_LOG(pc << ": loadc " << operand << std::endl);
      return invoke_loadc(env, operand);
    }
    case OpCode::LOADN:
      DEBUG_LOG(pc << ": loadn" << std::endl);
      return invoke_loadn(env);
    case OpCode::LOADP: {
      auto operand = TRY(stack->ReadOperand(frame));
      DEBUG_LOG(pc << ": loadp " << operand << std::endl);
      return invoke_loadp(env, operand);
    }
    case OpCode::LOADR: {
      auto operand = TRY(stack->ReadOperand(frame));
      DEBUG_LOG(pc << ": loadr " << operand << std::endl);
      return invoke_loadr(env, operand);
    }
    case OpCode::STOREP: {
      auto operand = TRY(stack->ReadOperand(frame));
      DEBUG_LOG(pc << ": storep " << operand << std::endl);
      return invoke_storep(env, operand);
    }
    case OpCode::STORER: {
      auto operand = TRY(stack->ReadOperand(frame));
      DEBUG_LOG(pc << ": storer " << operand << std::endl);
      return invoke_storer(env, operand);
    }
    case OpCode::CALL:
      DEBUG_LOG(pc << ": call" << std::endl);
      return invoke_call(env);
    case OpCode::RET:
      DEBUG_LOG(pc << ": ret" << std::endl);
      return invoke_ret(env);
    case OpCode::IF: {
      auto operand = TRY(stack->ReadOperand(frame));
      DEBUG_LOG(pc << ": if " << operand << std::endl);
      return invoke_if(env, operand);
    }
    case OpCode::GOTO: {
      auto operand = TRY(stack->ReadOperand(frame));
      DEBUG_LOG(pc << ": goto " << operand << std::endl);
      return invoke_goto(env, operand);
    }
    case OpCode::NEW:
      DEBUG_LOG(pc << ": new" << std::endl);
      return invoke_new(env);
    case OpCode::SINGLETON:
      DEBUG_LOG(pc << ": singleton" << std::endl);
      return invoke_singleton(env);
    case OpCode::TYPEOF:
      DEBUG_LOG(pc << ": typeof" << std::endl);
      return invoke_typeof(env);
    case OpCode::GETP:
      DEBUG_LOG(pc << ": getp" << std::endl);
      return invoke_getp(env);
    case OpCode::GETR:
      DEBUG_LOG(pc << ": getr" << std::endl);
      return invoke_getr(env);
    case OpCode::SETP:
      DEBUG_LOG(pc << ": setp" << std::endl);
      return invoke_setp(env);
    case OpCode::SETR:
      DEBUG_LOG(pc << ": setr" << std::endl);
      return invoke_setr(env);

    case OpCode::EQ_REF:
      DEBUG_LOG(pc << ": eq_ref" << std::endl);
      return invoke_eq_ref(env);

    case OpCode::NEG_INT64:
      DEBUG_LOG(pc << ": neg_int64" << std::endl);
      return invoke_neg_int64(env);
    case OpCode::ADD_INT64:
      DEBUG_LOG(pc << ": add_int64" << std::endl);
      return invoke_add_int64(env);
    case OpCode::SUB_INT64:
      DEBUG_LOG(pc << ": sub_int64" << std::endl);
      return invoke_sub_int64(env);
    case OpCode::MUL_INT64:
      DEBUG_LOG(pc << ": mul_int64" << std::endl);
      return invoke_mul_int64(env);
    case OpCode::DIV_INT64:
      DEBUG_LOG(pc << ": div_int64" << std::endl);
      return invoke_div_int64(env);
    case OpCode::MOD_INT64:
      DEBUG_LOG(pc << ": mod_int64" << std::endl);
      return invoke_mod_int64(env);

    case OpCode::NOT_INT64:
      DEBUG_LOG(pc << ": not_int64" << std::endl);
      return invoke_not_int64(env);
    case OpCode::AND_INT64:
      DEBUG_LOG(pc << ": and_int64" << std::endl);
      return invoke_and_int64(env);
    case OpCode::OR_INT64:
      DEBUG_LOG(pc << ": or_int64" << std::endl);
      return invoke_or_int64(env);
    case OpCode::XOR_INT64:
      DEBUG_LOG(pc << ": xor_int64" << std::endl);
      return invoke_xor_int64(env);
    case OpCode::SHL_INT64:
      DEBUG_LOG(pc << ": shl_int64" << std::endl);
      return invoke_shl_int64(env);
    case OpCode::SHR_INT64:
      DEBUG_LOG(pc << ": shr_int64" << std::endl);
      return invoke_shr_int64(env);

    case OpCode::EQ_INT64:
      DEBUG_LOG(pc << ": eq_int64" << std::endl);
      return invoke_eq_int64(env);
    case OpCode::NE_INT64:
      DEBUG_LOG(pc << ": ne_int64" << std::endl);
      return invoke_ne_int64(env);
    case OpCode::GT_INT64:
      DEBUG_LOG(pc << ": gt_int64" << std::endl);
      return invoke_gt_int64(env);
    case OpCode::GE_INT64:
      DEBUG_LOG(pc << ": ge_int64" << std::endl);
      return invoke_ge_int64(env);
    case OpCode::LT_INT64:
      DEBUG_LOG(pc << ": lt_int64" << std::endl);
      return invoke_lt_int64(env);
    case OpCode::LE_INT64:
      DEBUG_LOG(pc << ": le_int64" << std::endl);
      return invoke_le_int64(env);
    default:
      return make_error("Unknown op: " + to_string(static_cast<int>(op)));
  }
}
Result<Value> Engine::CallProcedure(Environment* env,
                                    const std::string& proc_name,
                                    const std::vector<Value>& params) {
  std::string m, p;
  std::tie(m, p) = TRY(parse_procedure(proc_name));
  auto module = TRY(env->GetModule(m));
  auto proc = TRY(module->GetProcedure(p));
  auto&& assembly = proc->assembly();
  auto stack = env->call_stack();
  auto initial_frame = TRY(stack->CurrentFrame());

  // 1. 将参数压栈
  CountT param_count = proc->assembly().ParamCount();
  if (param_count != params.size()) {
    return make_error("Incorrect param count: " + to_string(params.size()));
  }
  for (CountT i = 0; i < param_count; ++i) {
    if (TRY(proc->assembly().GetParam(i)) != ValueType::REFERENCE) {
      TRY(stack->PushPrimitive(initial_frame, params[i].primitive));
    } else {
      TRY(stack->PushReference(initial_frame, params[i].reference));
    }
  }

  // 2. 将函数引用压栈
  TRY(stack->PushPrimitive(initial_frame, { .procedure_value = proc.get() }));

  // 3. 执行代码
  TRY(invoke_call(env)); // 插入执行 call 指令
  while (TRY(stack->CurrentFrame()) != initial_frame) {
    TRY(InvokeOp(env));
  }

  // 4. 获取返回值
  switch (assembly.ret_type()) {
    case assembly::Procedure::RetType::VOID:
      return Value {};
    case assembly::Procedure::RetType::REFERENCE:
      return Value {.reference = TRY(stack->PopReference(initial_frame)) };
    default:
      return Value {.primitive = TRY(stack->PopPrimitive(initial_frame)) };
  }
}

}
