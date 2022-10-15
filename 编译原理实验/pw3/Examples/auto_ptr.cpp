#include<iostream>
#include<memory>	//auto_ptr的头文件
#include<cassert>
using namespace std;
class Test
{
public:
    Test(string s)
    {
        str = s;
       cout<<"Test creat\n";
    }
    ~Test()
    {
        cout<<"Test delete:"<<str<<endl;
    }
    string& getStr()
    {
        return str;
    }
    void setStr(string s)
    {
        str = s;
    }
    void print()
    {
        cout<<str<<endl;
    }
private:
    string str;
};
  
void func(auto_ptr<Test> ptest){
    return;
}
  
int main()
{
    auto_ptr<Test> ptest(new Test("123"));	//调用构造函数输出Test creat
    ptest->setStr("hello ");				//修改成员变量的值
    ptest->print();							//输出hello
    ptest.get()->print();					//输出hello
    ptest->getStr() += "world !";
    (*ptest).print();						//输出hello world
    ptest.reset(new Test("123"));//成员函数reset()重新绑定指向的对象，而原来的对象则会被释放，所以这里会调用一次构造函数，还有调用一次析构函数释放掉之前的对象
    ptest->print();							//输出123
    func(ptest);
    ptest->print();
    assert(ptest.get()  && "it is NULL now");
    return 0;								//此时还剩下一个对象，调用一次析构函数释放该对象
}