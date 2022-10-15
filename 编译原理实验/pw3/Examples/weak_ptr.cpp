#include<iostream>
#include<memory>
using namespace std;

template <typename T>
class B;

class A
{
public:
    shared_ptr<B<A>> pb_;
    ~A()
    {
        cout<<"A delete\n";
    }
};

class A1
{
public:
    weak_ptr<B<A1>> pb_;
    ~A1()
    {
        cout<<"A1 delete\n";
    }
};

template <typename T>
class B
{
public:
    shared_ptr<T> pa_;
    ~B()
    {
        cout<<"B delete\n";
    }
};
  
void fun()
{
    shared_ptr<B<A>> pb(new B<A>());
    shared_ptr<A> pa(new A());
    pb->pa_ = pa;
    pa->pb_ = pb;
    cout<<"fun : "<<pb.use_count()<<endl;
    cout<<"fun : "<<pa.use_count()<<endl;
    // 这个函数执行完会出现相互引用导致的内存泄漏
}

void fun1()
{
    shared_ptr<B<A1>> pb(new B<A1>());
    shared_ptr<A1> pa(new A1());
    pb->pa_ = pa;
    pa->pb_ = pb;
    cout<<"fun1 : "<<pb.use_count()<<endl;
    cout<<"fun1 : "<<pa.use_count()<<endl;
}

int main()
{
    fun();
    fun1();
    return 0;
}