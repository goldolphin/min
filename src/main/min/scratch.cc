#include <iostream>
#include <string>

int order() {
  static int order = 0;
  return ++order;
}

template <class T>
T gen();

template<>
int gen<int>() {
  return order();
}

template<>
std::string gen<std::string>() {
  return "S" + std::to_string(order());
}


void foo(int a, int b, std::string c) {
  std::cout << a << ", " << b << ", " << c << std::endl;
}

template <auto base, class Func>
struct Helper {

};

template <auto base, class R, class A0, class... As>
struct Helper<base, R(A0, As...)> {
  template<class... Bs>
  static R func(Bs... bs) {
    return Helper<base, R(As...)>::template func<Bs..., A0>(bs..., gen<A0>());
  }
};

template <auto base, class R>
struct Helper<base, R()> {
  template<class... Bs>
  static R func(Bs... bs) {
    return base(bs...);
  }
};

struct CollectableHeader {
 public:
  enum class Flag : char {
    UNKNOWN = 0,
    REACHABLE = 1,
  };

  int* type;
  Flag flag;
};

struct CollectableData {
  char data[0];
};

struct Collectable : public CollectableHeader, public CollectableData {

};


int main() {
  std::cout << sizeof(CollectableHeader) << std::endl;
  std::cout << sizeof(CollectableData) << std::endl;
  std::cout << sizeof(Collectable) << std::endl;
  std::cout << offsetof(Collectable, type) << std::endl;
  std::cout << offsetof(Collectable, flag) << std::endl;

  Collectable o;
  CollectableHeader* h = &o;
  CollectableData* d = &o;
  auto* c = static_cast<Collectable*>(d);
  std::cout << std::hex << &o << std::endl;
  std::cout << std::hex << h << std::endl;
  std::cout << std::hex << d << std::endl;
  std::cout << std::hex << c << std::endl;
  Helper<foo, decltype(foo)>::func();
  void* mem = h;
  static_cast<CollectableData*>(mem)->~CollectableData();
  return 0;
}
