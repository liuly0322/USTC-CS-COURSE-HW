# SysYF runtime

SysYF运行时库提供一系列I/O函数用于在SysYF程序中表达输入/输出功能需求。这些库函数不用在SysYF程序中声明即可在SysYF程序中的函数里调用。这些库函数支持对整数、浮点数、字符以及一串整数或浮点数的输入和输出。

- [SysYF runtime](#sysyf-runtime)
  - [getint](#getint)
  - [getfloat](#getfloat)
  - [getch](#getch)
  - [getarray](#getarray)
  - [getfarray](#getfarray)
  - [putint](#putint)
  - [putfloat](#putfloat)
  - [putch](#putch)
  - [putarray](#putarray)
  - [putfarray](#putfarray)


## getint

int getint()

输入一个整数，返回对应的整数值。

**示例**：int n; n = getint();

## getfloat

float getfloat()  

输入一个浮点数，返回对应的浮点数值。

**示例**：float n; n = getfloat(); 

## getch

int getch()  

输入一个字符，返回字符对应的ASCII码值。

**示例**：int n; n = getch();

## getarray

int getarray(int[])

输入一串整数，第 1 个整数代表后续要输入的整数个数，该个数通过返回值返回；后续的整数通过传入的数组参数返回。  

**注**：getarray 函数获取传入的数组的起始地址，不检查调用者提供的数组是否有足够的空间容纳输入的一串整数。  

**示例**：int a[10][10];int n; n = getarray(a[0]);

## getfarray

int getfarray(float[])  

输入一个整数后跟若干个浮点数，第 1 个整数代表后续要输入的浮点数个数，该个数通过返回值返回；后续的浮点数通过传入的数组参数返回。  

**注**：getfarray 函数获取传入的数组的起始地址，不检查调用者提供的数组是否有足够的空间容纳输入的一串整数。  

**示例**：float a[10][10]; int n; n = getfarray(a[0]); 

## putint

void putint(int)  

输出一个整数的值。  

**示例**：int n=10; putint(n); putint(10); putint(n); 
**输出**：101010

## putfloat

void putfloat(float)  

输出一个浮点数的值。  

**示例**：float n=10.0; putfloat(n); putfloat(10.0);putfloat(n); 
**输出**：10.00000010.0000010.000000

## putch

void putch(int)  

将整数参数的值作为 ASCII 码，输出该 ASCII 码对应的字符。  

**注**：传入的整数参数取值范围为 0~255，putch()不检查参数的合法性。

**示例**：int n=10; putch(n);  
**输出**：换行符

## putarray

void putarray(int,int[])  

第 1 个参数表示要输出的整数个数(假设为 N)，后面应该跟上要输出的 N个整数的数组。putarray 在输出时会在整数之间安插空格。  

**注**：putarray 函数不检查参数的合法性。  

**示例**：int n=2; int a[2]={2,3}; putarray(n, a);  
**输出**：2: 2 3

## putfarray

void putfarray(int,float[])

第 1 个参数表示要输出的浮点数个数(假设为 N)，后面应该跟上要输出的N个整数的数组。putfarray 在输出时会在浮点数之间安插空格。  

**注**：putfarray 函数不检查参数的合法性。  

**示例**：int n=2; float a[2]={2.0,3.0}; putfarray(n, a);  
**输出**：2: 2.000000 3.000000