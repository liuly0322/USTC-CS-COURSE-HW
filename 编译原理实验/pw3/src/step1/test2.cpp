#include <iostream>
#include <memory>

class Foo {
  public:
    char *bar;
    Foo() : bar(new char[100]) {}
    ~Foo() { delete[] bar; }
};

void leak() {
    constexpr auto size = 1000;
    // calling delete on Foo[]* is ub, there may be different results.
    // this example may cause memory leak by:
    // only the deconstructor of Foo[0] is called,
    // while deconstructor of other instances of Foo are not called.
    std::auto_ptr<Foo> foo(new Foo[size]);
}

int main() {
    // not so much memory, works fine
    leak();
    std::cout << "Calling once is OK" << std::endl;

    // memory leak
    constexpr auto iter_count = 10000;
    for (auto i = 0; i < iter_count; i++) {
        leak();
    }
    std::cout << "You can't reach here" << std::endl;
}