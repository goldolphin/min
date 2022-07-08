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


int main() {
  Helper<foo, decltype(foo)>::func();
  return 0;
}
