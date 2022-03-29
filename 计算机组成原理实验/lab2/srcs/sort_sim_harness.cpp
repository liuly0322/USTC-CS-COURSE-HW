// sort_sim_harness.cpp
#include <verilated.h>        // 核心头文件
#include <verilated_vcd_c.h>  // 波形生成头文件
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "Vsort_sim.h"  // 译码器模块类
using namespace std;

Vsort_sim* top;      // 顶层dut对象指针
VerilatedVcdC* tfp;  // 波形生成对象指针

vluint64_t main_time = 0;            // 仿真时间戳
const vluint64_t sim_time = 700000;  // 最大仿真时间戳
                                     // 这里考虑的是 6w 周期

int get_vd(vluint64_t time) {
    // 每上升沿周围保持稳定（如 5 到 15）
    // 返回的是一个 32 位的随机数
    if (time >= 2565)
        return 0;
    static int res = 0;
    if (time % 10 == 5)
        res = rand() + (rand() % 2 ? (1 << 31) : 0);
    return res;
}

int main(int argc, char** argv) {
    // 一些初始化工作
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // 为对象分配内存空间
    top = new Vsort_sim;
    tfp = new VerilatedVcdC;

    // tfp初始化工作
    top->trace(tfp, 99);
    tfp->open("sort_sim.vcd");

    int count = 0;

    while (!Verilated::gotFinish() && main_time < sim_time) {
        // 仿真过程
        top->clk = (main_time % 10) < 5 ? 1 : 0;  // 时钟
        top->rstn = main_time <= 1 ? 0 : 1;       // 初始复位
        // 一开始需要 256 个周期写入随机数，2560 tick
        top->run = (main_time > 2565 && main_time < 2575) ? 1 : 0;
        top->init = (main_time < 2565) ? 1 : 0;
        top->v_we = 1;
        top->v_a =
            (main_time > 5 && main_time < 2565) ? (main_time - 5) / 10 : 0;
        top->v_d = get_vd(main_time);

        top->eval();
        tfp->dump(main_time);
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