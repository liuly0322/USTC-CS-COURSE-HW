#include <iostream>
#include <memory>

class Foo {
  public:
    Foo(int a) : a(a) {}
    int a;
};

void print_foo(std::auto_ptr<Foo> foo) {
    // borrow the ownership of foo and not giving back
    std::cout << foo->a << std::endl;
}

int main() {
    std::auto_ptr<Foo> foo(new Foo(1));

    // the ownership of foo will be borrowed
    print_foo(foo);

    // now, foo is a nullptr. The output should be 0
    std::cout << (unsigned long)foo.get() << std::endl;

    // the following will cause segmentation fault
    // std::cout << foo->a;
}