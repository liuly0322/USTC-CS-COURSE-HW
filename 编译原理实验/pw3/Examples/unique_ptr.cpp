#include<iostream>
#include<memory>
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
unique_ptr<Test> fun()
{
    return unique_ptr<Test>(new Test("789"));//调用了构造函数，输出 Test creat
}

void func(unique_ptr<Test> & ptest){
    return;
} 

int main()
{
    unique_ptr<Test> ptest(new Test("123"));	//调用构造函数，输出 Test creat
    func(ptest);
    unique_ptr<Test> ptest2(new Test("456"));	//调用构造函数，输出 Test creat
    ptest->print();								//输出 123
    ptest2 = std::move(ptest);	//不能直接 ptest2 = ptest，调用了 move 后 ptest2 原本的对象会被释放，ptest2 对象指向原本 ptest 对象的内存，输出 Test delete 456
    if(ptest == NULL)cout<<"ptest = NULL\n";	//因为两个 unique_ptr 不能指向同一内存地址，所以经过前面 move 后 ptest 会被赋值 NULL，输出 ptest=NULL
    Test* p = ptest2.release();	//release 成员函数把 ptest2 指针赋为空，但是并没有释放指针指向的内存，所以此时 p 指针指向原本 ptest2 指向的内存
    p->print();					//输出 123
    ptest.reset(p);				//重新绑定对象，原来的对象会被释放掉，但是 ptest 对象本来就释放过了，所以这里就不会再调用析构函数了
    ptest->print();				//输出 123
    ptest2 = fun(); 			//这里可以用=，因为使用了移动构造函数，函数返回一个 unique_ptr 会自动调用移动构造函数
    ptest2->print();			//输出 789
    return 0;					//此时程序中还有两个对象，调用两次析构函数释放对象
}