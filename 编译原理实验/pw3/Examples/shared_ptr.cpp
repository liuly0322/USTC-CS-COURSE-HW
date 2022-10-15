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
    return unique_ptr<Test>(new Test("789"));
}
int main()
{
    shared_ptr<Test> ptest(new Test("123"));	//调用构造函数输出Test create
    shared_ptr<Test> ptest2(new Test("456"));	//调用构造函数输出 Test creat
    cout<<ptest2->getStr()<<endl;				//输出456
    cout<<ptest2.use_count()<<endl;				//显示此时资源被几个指针共享，输出1
    ptest = ptest2;		//"456"引用次数加1，“123”销毁，输出Test delete：123
    ptest->print();		//输出456
    cout<<ptest2.use_count()<<endl;				//该指针指向的资源被几个指针共享，输出2
    cout<<ptest.use_count()<<endl;				//2
    ptest.reset();	//重新绑定对象，绑定一个空对象，当时此时指针指向的对象还有其他指针能指向就不会释放该对象的内存空间，
    ptest2.reset();	//此时“456”销毁，此时指针指向的内存空间上的指针为0，就释放了该内存，输出Test delete
    cout<<"done !\n";
    return 0;
}