#include "min/vm/engine.h"
#include "min/vm/op_writer.h"
#include <iostream>

using namespace min;

#define TEST(case) ({                                      \
  std::cout << "=== Run " << #case << " ===" << std::endl; \
  ENSURE(case());                                          \
})

Result<void> test_call() {
  // 定义代码
  Environment env;
  TRY(env.NewModule("test"));
  auto module = TRY(env.GetModule("test"));
  TRY(module->DefineProcedure(assembly::Procedure("add",
                                                  min::assembly::Procedure::RetType::INT64,
                                                  { ValueType::INT64, ValueType::INT64 },
                                                  { },
                                                  TRY(OpWriter()
                                                  .Write_add_int64()
                                                  .Write_ret()
                                                  .ToByteCodes(module, env)))));
  TRY(module->DefineProcedure(assembly::Procedure("mul4",
                                                  min::assembly::Procedure::RetType::INT64,
                                                  { ValueType::INT64 },
                                                  { ValueType::INT64 },
                                                  TRY(OpWriter()
                                                                .Write_storep(0)
                                                                .Write_loadp(0)
                                                                .Write_loadp(0)
                                                                .Write_loadc({ PrimitiveType::PROCEDURE, "test.add" })
                                                                .Write_call()
                                                                .Write_storep(0)
                                                                .Write_loadp(0)
                                                                .Write_loadp(0)
                                                                .Write_loadc({ PrimitiveType::PROCEDURE, "test.add" })
                                                                .Write_call()
                                                                .Write_ret()
                                                                .ToByteCodes(module, env)))));

  // 执行
  Engine engine;
  Int64T a = 1;
  Int64T b = 2;
  auto v1 = TRY(engine.CallProcedure(&env, "test.add", { {{.int64_value = a}}, {{.int64_value = b}} }));
  std::cout << v1.primitive.int64_value << std::endl;

  auto v2 = TRY(engine.CallProcedure(&env, "test.mul4", {{ v1.primitive }}));
  std::cout << v2.primitive.int64_value << std::endl;

  auto v3 = TRY(engine.CallProcedure(&env, "test.mul4", {{ v2.primitive }}));
  std::cout << v3.primitive.int64_value << std::endl;

  return {};
}

class PrintImpl : public native::Procedure {
 public:
  Result<void> Call(Environment* env) const override {
    auto stack = env->call_stack();
    auto frame = TRY(stack->CurrentFrame());
    auto v = TRY(stack->PopPrimitive(frame));
    std::cout << "print: " << v.int64_value << std::endl;
    return {};
  }
};

Result<void> test_list() {
  // 定义代码
  Environment env;
  TRY(env.NewModule("hello"));
  auto module = TRY(env.GetModule("hello"));
  assembly::Struct struct_node ("node");
  struct_node.Put({"value", ValueType::INT64});
  struct_node.Put({"next", ValueType::REFERENCE});
  TRY(module->DefineStruct(std::move(struct_node)));
  TRY(module->DefineProcedure(assembly::Procedure("print",
                                                  min::assembly::Procedure::RetType::VOID,
                                                  { ValueType::INT64 },
                                                  {}),
                              std::make_unique<PrintImpl>()));
  TRY(module->DefineProcedure(assembly::Procedure("main",
                                                  min::assembly::Procedure::RetType::VOID,
                                                  {},
                                                  { ValueType::REFERENCE, ValueType::REFERENCE, ValueType::REFERENCE },
                                                  TRY(OpWriter()
                                                                 // a = new hello.node
                                                                .Write_loadc({ PrimitiveType::TYPE, "hello.node" })
                                                                .Write_new()
                                                                .Write_storer(0)

                                                                // a.value = 1000
                                                                .Write_loadc({ PrimitiveType::INT64, "1000" })
                                                                .Write_loadc({ PrimitiveType::FIELD, "hello.node.value" })
                                                                .Write_loadr(0)
                                                                .Write_setp()

                                                                // a.next = null
                                                                .Write_loadn()
                                                                .Write_loadc({ PrimitiveType::FIELD, "hello.node.next" })
                                                                .Write_loadr(0)
                                                                .Write_setr()

                                                                // b = new hello.node
                                                                .Write_loadc({ PrimitiveType::TYPE, "hello.node" })
                                                                .Write_new()
                                                                .Write_storer(1)

                                                                // b.value = 2000
                                                                .Write_loadc({ PrimitiveType::INT64, "2000" })
                                                                .Write_loadc({ PrimitiveType::FIELD, "hello.node.value" })
                                                                .Write_loadr(1)
                                                                .Write_setp()

                                                                // b.next = a
                                                                .Write_loadr(0)
                                                                .Write_loadc({ PrimitiveType::FIELD, "hello.node.next" })
                                                                .Write_loadr(1)
                                                                .Write_setr()

                                                                // head = b
                                                                .Write_loadr(1)
                                                                .Write_storer(2)

                                                                // if (head == null) break
                                                                .DefineLabel("BEGIN")
                                                                .Write_loadn()
                                                                .Write_loadr(2)
                                                                .Write_eq_ref()
                                                                .Write_if("END")

                                                                // print(head.value)
                                                                .Write_loadc({ PrimitiveType::FIELD, "hello.node.value" })
                                                                .Write_loadr(2)
                                                                .Write_getp()
                                                                .Write_loadc({ PrimitiveType::PROCEDURE, "hello.print" })
                                                                .Write_call()

                                                                // head = head.next
                                                                .Write_loadc({ PrimitiveType::FIELD, "hello.node.next" })
                                                                .Write_loadr(2)
                                                                .Write_getr()
                                                                .Write_storer(2)

                                                                .Write_goto("BEGIN")
                                                                .DefineLabel("END")
                                                                .Write_ret()
                                                                .ToByteCodes(module, env)))));

  // 执行
  Engine engine;
  return engine.CallProcedure(&env, "hello.main", {});
}

Result<void> test_gc() {
  // 定义代码
  auto options = Options::Default();
  options.heap_options.capacity = 1000;
  Environment env(std::move(options));
  TRY(env.NewModule("test"));
  auto module = TRY(env.GetModule("test"));
  assembly::Struct struct_node ("node");
  struct_node.Put({"value", ValueType::INT64});
  struct_node.Put({"next", ValueType::REFERENCE});
  TRY(module->DefineStruct(std::move(struct_node)));
  TRY(module->DefineProcedure(assembly::Procedure("main",
                                                  min::assembly::Procedure::RetType::VOID,
                                                  { },
                                                  { ValueType::REFERENCE },
                                                  TRY(OpWriter()
                                                          .DefineLabel("BEGIN")
                                                          .Write_loadc({ PrimitiveType::TYPE, "test.node" })
                                                          .Write_new()
                                                          .Write_storer(0)
                                                          .Write_loadc({ PrimitiveType::TYPE, "test.node" })
                                                          .Write_new()
                                                          .Write_goto("BEGIN")
                                                          .Write_ret()
                                                          .ToByteCodes(module, env)))));

  // 执行
  Engine engine;
  Int64T a = 1;
  Int64T b = 2;
  return engine.CallProcedure(&env, "test.main", {});
}

int main() {
  TEST(test_call);
  TEST(test_list);
  TEST(test_gc);
  return 0;
}
