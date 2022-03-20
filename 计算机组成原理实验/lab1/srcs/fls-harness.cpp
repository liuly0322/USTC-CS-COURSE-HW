// fls-harness.cpp
#include <verilated.h>        // 核心头文件
#include <verilated_vcd_c.h>  // 波形生成头文件
#include <fstream>
#include <iostream>
#include "Vfls.h"  // 译码器模块类
using namespace std;

Vfls* top;           // 顶层dut对象指针
VerilatedVcdC* tfp;  // 波形生成对象指针

vluint64_t main_time = 0;          // 仿真时间戳
const vluint64_t sim_time = 1024;  // 最大仿真时间戳

int main(int argc, char** argv) {
    // 一些初始化工作
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // 为对象分配内存空间
    top = new Vfls;
    tfp = new VerilatedVcdC;

    // tfp初始化工作
    top->trace(tfp, 99);
    tfp->open("fls.vcd");

    // 初始化，默认有效
    top->rstn = 1;

    while (!Verilated::gotFinish() && main_time < sim_time / 2 + 1) {
        top->en = main_time % 11 < 2 ? 1 : 0;  // 每一定时间产生使能信号
        top->clk = main_time % 2;              // 模拟时钟
        top->d = main_time < 5 ? 1 : 2;        // 依次载入 1 和 2
        top->eval();                           // 仿真时间步进
        tfp->dump(main_time);                  // 波形文件写入步进
        main_time++;
    }

    top->clk = 1;
    top->rstn = 0;  // 测试复位
    top->eval();
    tfp->dump(main_time);
    main_time++;
    top->rstn = 1;

    while (!Verilated::gotFinish() && main_time < sim_time) {
        top->clk = main_time % 2;              // 模拟时钟
        top->en = main_time % 11 < 2 ? 1 : 0;  // 每一定时间产生使能信号
        top->eval();                           // 仿真时间步进
        tfp->dump(main_time);                  // 波形文件写入步进
        main_time++;
    }

    std::cout << "test finished\n";

    // 清理工作
    tfp->close();
    delete top;
    delete tfp;
    exit(0);
    return 0;
}