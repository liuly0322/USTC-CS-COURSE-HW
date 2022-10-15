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
    return unique_ptr<Test>(new Test("789"));//调用了构造函数，输出Test creat
}

void func(unique_ptr<Test> & ptest){
    return;
} 

int main()
{
    unique_ptr<Test> ptest(new Test("123"));	//调用构造函数，输出Test creat
    func(ptest);
    unique_ptr<Test> ptest2(new Test("456"));	//调用构造函数，输出Test creat
    ptest->print();								//输出123
    ptest2 = std::move(ptest);	//不能直接ptest2 = ptest，调用了move后ptest2原本的对象会被释放，ptest2对象指向原本ptest对象的内存，输出Test delete 456
    if(ptest == NULL)cout<<"ptest = NULL\n";	//因为两个unique_ptr不能指向同一内存地址，所以经过前面move后ptest会被赋值NULL，输出ptest=NULL
    Test* p = ptest2.release();	//release成员函数把ptest2指针赋为空，但是并没有释放指针指向的内存，所以此时p指针指向原本ptest2指向的内存
    p->print();					//输出123
    ptest.reset(p);				//重新绑定对象，原来的对象会被释放掉，但是ptest对象本来就释放过了，所以这里就不会再调用析构函数了
    ptest->print();				//输出123
    ptest2 = fun(); 			//这里可以用=，因为使用了移动构造函数，函数返回一个unique_ptr会自动调用移动构造函数
    ptest2->print();			//输出789
    return 0;					//此时程序中还有两个对象，调用两次析构函数释放对象
}