#include <iostream>
#include <memory>

void test1() {
    std::weak_ptr<int> p1;

    {
        auto p2 = std::make_shared<int>(42);
        p1 = p2;
        std::cout << p1.lock().get() << std::endl;
        std::cout << *p1.lock() << std::endl;
    }

    // the shared_ptr is expired
    std::cout << p1.expired() << std::endl;
    // you can only get nullptr
    std::cout << p1.lock().get() << std::endl;
    // so you can't access the resource
    // the following will cause segment fault
    // std::cout << *p1.lock() << std::endl;
}

void test2() {
    std::weak_ptr<int> p1;
    auto p2 = std::make_shared<int>(42);
    {
        auto p3 = p2;
        p1 = p3;
    }
    std::cout << p1.expired() << std::endl;
    std::cout << p1.lock().get() << std::endl;
    std::cout << *p1.lock() << std::endl;
}

int main() {
    std::cout << "Test1:" << std::endl;
    test1();
    std::cout << "Test2:" << std::endl;
    test2();
}