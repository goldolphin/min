#include <iostream>

struct Type {
  enum class Enum {
    A = 0,
    B
  };

  operator Enum() const {
    return v;
  }

  const Enum v;
};

struct RetType : Type {

};

struct ValueType : public RetType {

};

struct PrimitiveType : ValueType {

};

enum class E {
  A,
  B,
  C
};

enum class F {
  A,
};

struct F2E {
  F2E(F f) : f_(f) {}
  operator E() {
    return static_cast<E>(f_);
  }

 private:
  F f_;
};

#define M1_int(a) int a;
#define M2(a, b) M1_##b(a);
#define M3(a) M3(a)

#define LIST A(a) A(b) A(c)

enum class List {
#define A(a) a,
  LIST
#undef A
};

int main() {
  PrimitiveType p1 {};
  Type t1 = p1;

  Type::Enum a = static_cast<Type::Enum>(10);

  std::cout << sizeof(Type) << std::endl;
  std::cout << sizeof(PrimitiveType) << std::endl;
  std::cout << static_cast<int>(t1.v) << std::endl;
  std::cout << static_cast<int>(a) << std::endl;

  switch (p1) {
    case Type::Enum::A:
      std::cout << "A" << std::endl;
      break;
    case Type::Enum::B:
      std::cout << "B" << std::endl;
      break;
  }

  F2E f2e = F::A;
  E e = F2E(F::A);

  M2(aaa, int);
  aaa = 1;

#define A(a) #a,
  char* list[] = { LIST };
  return 0;
}
