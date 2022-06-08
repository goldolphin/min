#include "common/result.h"
#include <iostream>

using namespace min;
Result<int> inc_non_neg(int x) {
  if (x < 0) return make_error("negative");
  return x + 1;
}

Result<void> test0(int x) {
  std::cout << "inc " << x << " = " << TRY(inc_non_neg(x)) << std::endl;
  return {};
}

Result<void> test1(int x) {
  auto r = test0(x);
  if (!r.ok()) {
    std::cout << "\nerror: " << r.error().message() << std::endl;
  }
  return r;
}

Result<void> test() {
  auto a = test1(2);
  TRY(std::move(a));
  TRY(test1(0));
  TRY(test1(1));
  TRY(test1(-1));
  std::cout << "Hello, World!" << std::endl;
  return {};
}

int main() {
  test();
  return 0;
}
