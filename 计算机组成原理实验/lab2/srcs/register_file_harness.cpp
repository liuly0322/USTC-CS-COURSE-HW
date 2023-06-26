// register_file_harness.cpp
#include <verilated.h>          // 核心头文件
#include <verilated_vcd_c.h>    // 波形生成头文件
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "Vregister_file.h"           // 译码器模块类
using namespace std;

Vregister_file* top;                  // 顶层 dut 对象指针
VerilatedVcdC* tfp;             // 波形生成对象指针

vluint64_t main_time = 0;           // 仿真时间戳
const vluint64_t sim_time = 200;   // 最大仿真时间戳

// clk, ra0, ra1, wa, wd, we
int get_clk(vluint64_t time) {
    return time % 10 < 5 ? 1 : 0;
}

int get_ra0(vluint64_t time) {
    // 从 0 开始，每个周期递增
    return (time + 5) / 10;
}

int get_ra1(vluint64_t time) {
    // 一开始领先 ra0 一个周期
    if (time < 100)
        return (time + 15) / 10;
    // 后来查看原先内存
    return (time - 100) / 10;
}

int get_wa(vluint64_t time) {
    // 与 ra1 同步
    return (time + 15) / 10;
}

int get_wd(vluint64_t time) {
    // 与 wa 同步，wa 变动时生成随机数即可
    static int tmp = 0;
    if (time % 10 == 5)
        tmp = rand();
    return tmp;
}

int get_we(vluint64_t time) {
    // 取偶数上升沿
    int T_now = (time % 20);
    return (T_now > 3 && T_now < 13)? 1 : 0;
}

int main(int argc, char **argv)
{
    // 一些初始化工作
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // 为对象分配内存空间
    top = new Vregister_file;
    tfp = new VerilatedVcdC;

    // tfp 初始化工作
    top->trace(tfp, 99);
    tfp->open("register_file.vcd");

    int count = 0;

    while(!Verilated::gotFinish() && main_time < sim_time)
    {
        // 仿真过程
        // clk, ra0, ra1, wa, wd, we
        top->clk = get_clk(main_time);
        top->ra0 = get_ra0(main_time);
        top->ra1 = get_ra1(main_time);
        top->wa = get_wa(main_time);
        top->wd = get_wd(main_time);
        top->we = get_we(main_time);
        top->eval();            // 仿真时间步进
        tfp->dump(main_time);   // 波形文件写入步进
        count++;
        main_time++;
    }
    
    // 清理工作
    tfp->close();
    delete top;
    delete tfp;
    exit(0);
    return 0;
}