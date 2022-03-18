// Decoder-harness.cpp
#include <verilated.h>        // 核心头文件
#include <verilated_vcd_c.h>  // 波形生成头文件
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "Valu.h"  // 译码器模块类
using namespace std;

Valu* top;           // 顶层dut对象指针
VerilatedVcdC* tfp;  // 波形生成对象指针

vluint64_t main_time = 0;          // 仿真时间戳
const vluint64_t sim_time = 1024;  // 最大仿真时间戳

int get_rand_i32() {
    return (rand() & (1 << 31)) + rand();
}

int _minus(int a, int b) {
    return a - b;
}

int _add(int a, int b) {
    return a + b;
}

int _and(int a, int b) {
    return a & b;
}

int _or(int a, int b) {
    return a | b;
}

int _xor(int a, int b) {
    return a ^ b;
}

int l_rshift(int a, int b) {
    if (b >= 0 && b < 32)
        return (unsigned)a >> b;
    return 0;
}

int lshift(int a, int b) {
    if (b >= 0 && b < 32)
        return a << b;
    return 0;
}

int a_rshift(int a, int b) {
    if (b >= 0 && b < 32)
        return a >> b;
    return 0;
}

int cmp_f(int a, int b) {
    // 类似实现 f 模块的功能
    int res = 0;
    if (a == b)
        res += 1;
    if (a < b)
        res += 2;
    if ((unsigned)a < (unsigned)b) {
        res += 4;
    }
    return res;
}

int main(int argc, char** argv) {
    // 一些初始化工作
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // 为对象分配内存空间
    top = new Valu;
    tfp = new VerilatedVcdC;

    // tfp初始化工作
    top->trace(tfp, 99);
    tfp->open("Decoder.vcd");

    // 生成随机数（固定种子确保可复现）
    srand(1024);

    // 功能函数
    int (*a[8])(int, int) = {_minus, _add,     _and,   _or,
                             _xor,   l_rshift, lshift, a_rshift};

    // 测试每一种功能
    for (int s = 0; s < 8; s++) {
        // 测十次
        for (int i = 0; i < 10; i++) {
            top->a = get_rand_i32();
            top->b = get_rand_i32();
            top->s = s;
            top->eval();
            if (top->y != a[s](top->a, top->b)) {
                cout << "error test case\n";
                cout << "mode: " << s;
            }
            if (s == 0) {
                if (top->f != cmp_f(top->a, top->b)) {
                    cout << "error cmp\n";
                }
            }
            tfp->dump(main_time);
            main_time++;
        }
    }

    std::cout << "test finished\n";

    // 清理工作
    tfp->close();
    delete top;
    delete tfp;
    exit(0);
    return 0;
}