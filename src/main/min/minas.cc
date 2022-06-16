//
// Created by goldolphin on 2022/6/10.
//

#include "min/io/assembly_interpreter.h"
#include "min/common/to_string.h"

using namespace min;

Result<void> run(int argc, char** argv) {
  if (argc < 1) {
    return make_error("Invalid arguments.\nUsage: " + to_string(argv[0]) + " run | compile\n");
  }
  auto mode = argv[1];

  AssemblyInterpreterHandler handler;
  AssemblyReader reader(&handler, &std::cin);
  return reader.Process();
}

int main(int argc, char** argv) {
  ENSURE(run(argc, argv));
  return 0;
}
