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

  assembly::Procedure proc_add("add");
  proc_add.ret_type(RetType::INT64);
  proc_add.SetParams({ ValueType::INT64, ValueType::INT64 });
  proc_add.SetByteCodes(TRY(OpWriter()
                                .Write_add_int64()
                                .Write_ret()
                                .ToByteCodes(module, env)));
  TRY(module->DefineProcedure(std::move(proc_add)));

  assembly::Procedure proc_mul4("mul4");
  proc_mul4.ret_type(RetType::INT64);
  proc_mul4.SetParams({ ValueType::INT64 });
  proc_mul4.SetLocals({ ValueType::INT64 });
  proc_mul4.SetByteCodes(TRY(OpWriter()
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
                                 .ToByteCodes(module, env)));
  TRY(module->DefineProcedure(std::move(proc_mul4)));

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

  assembly::Procedure proc_print("print");
  proc_print.SetParams({ ValueType::INT64 });
  TRY(module->DefineProcedure(std::move(proc_print),
                              std::make_unique<PrintImpl>()));

  assembly::Procedure proc_main("main");
  proc_main.SetLocals({ ValueType::REFERENCE, ValueType::REFERENCE, ValueType::REFERENCE });
  proc_main.SetByteCodes(TRY(OpWriter()
                                 // a = new hello.node
                                 .Write_loadc({PrimitiveType::TYPE, "hello.node"})
                                 .Write_new()
                                 .Write_storer(0)

                                     // a.value = 1000
                                 .Write_loadc({PrimitiveType::INT64, "1000"})
                                 .Write_loadc({PrimitiveType::FIELD, "hello.node.value"})
                                 .Write_loadr(0)
                                 .Write_setp()

                                     // a.next = null
                                 .Write_loadn()
                                 .Write_loadc({PrimitiveType::FIELD, "hello.node.next"})
                                 .Write_loadr(0)
                                 .Write_setr()

                                     // b = new hello.node
                                 .Write_loadc({PrimitiveType::TYPE, "hello.node"})
                                 .Write_new()
                                 .Write_storer(1)

                                     // b.value = 2000
                                 .Write_loadc({PrimitiveType::INT64, "2000"})
                                 .Write_loadc({PrimitiveType::FIELD, "hello.node.value"})
                                 .Write_loadr(1)
                                 .Write_setp()

                                     // b.next = a
                                 .Write_loadr(0)
                                 .Write_loadc({PrimitiveType::FIELD, "hello.node.next"})
                                 .Write_loadr(1)
                                 .Write_setr()

                                     // head = b
                                 .Write_loadr(1)
                                 .Write_storer(2)

                                     // if (head == null) break
                                 .label("BEGIN")
                                 .Write_loadn()
                                 .Write_loadr(2)
                                 .Write_eq_ref()
                                 .Write_if("END")

                                     // print(head.value)
                                 .Write_loadc({PrimitiveType::FIELD, "hello.node.value"})
                                 .Write_loadr(2)
                                 .Write_getp()
                                 .Write_loadc({PrimitiveType::PROCEDURE, "hello.print"})
                                 .Write_call()

                                     // head = head.next
                                 .Write_loadc({PrimitiveType::FIELD, "hello.node.next"})
                                 .Write_loadr(2)
                                 .Write_getr()
                                 .Write_storer(2)

                                 .Write_goto("BEGIN")
                                 .label("END")
                                 .Write_ret()
                                 .ToByteCodes(module, env)));
  TRY(module->DefineProcedure(std::move(proc_main)));

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

  assembly::Procedure proc_main("main");
  proc_main.SetLocals({ ValueType::REFERENCE });
  proc_main.SetByteCodes(TRY(OpWriter()
                                 .label("BEGIN")
                                 .Write_loadc({ PrimitiveType::TYPE, "test.node" })
                                 .Write_new()
                                 .Write_storer(0)
                                 .Write_loadc({ PrimitiveType::TYPE, "test.node" })
                                 .Write_new()
                                 .Write_goto("BEGIN")
                                 .Write_ret()
                                 .ToByteCodes(module, env)));
  TRY(module->DefineProcedure(std::move(proc_main)));

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
