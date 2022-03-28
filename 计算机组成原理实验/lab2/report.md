&nbsp;

<div style="text-align:center;font-size:2.5em;font-weight:bold">中国科学技术大学计算机学院</div>

&nbsp;

<div style="text-align:center;font-size:2.5em;font-weight:bold">《计算机组成原理实验报告》</div>

&nbsp;

&nbsp;

&nbsp;

&nbsp;

&nbsp;

<img src="../logo.png" style="zoom: 50%;" />

&nbsp;

&nbsp;

&nbsp;

&nbsp;

<div style="display:flex;justify-content:center;font-size:2em">
<div>
<p>实验题目：寄存器堆与存储器及其应用</p>
<p>学生姓名：刘良宇</p>
<p>学生学号：PB20000180</p>
<p>完成时间：2022. 3. 27</p>
</div>
</div>

<div style="page-break-after:always"></div>

## 实验题目

寄存器堆与存储器及其应用

## 实验目的

- 掌握寄存器堆和存储器的功能、时序及其应用
- 熟练掌握数据通路和控制器的设计和描述方法

## 实验环境

- VLAB：vlab.ustc.eud.cn
- Vivado
- Nexys4DDR

## 实验步骤

### 寄存器堆功能仿真

此部分需要完成 32x32 位的寄存器堆的功能仿真

- 寄存器堆的 0 号寄存器内容恒定为零
- 寄存器堆的写操作优先于读操作

在原有寄存器堆代码上稍作修改即可：

```verilog
// 保证异步读能做到写优先
assign rd0 = (ra0 == wa) && we ? wd : rf[ra0];
assign rd1 = (ra1 == wa) && we ? wd : rf[ra1];

// 0 号寄存器设置
always @(*) begin
    rf[0] = 0;
end

// 同步写
always @(posedge clk)
    if (wa !== 0 && we)
        rf[wa] <= wd;
```

完成之后即可进行仿真。这里使用 Verilator, 具体仿真文件及波形可以见附件，这里贴上最后波形：

根据 `we` ，依次写入 02，04，06 等位置，这里可以观察到 `rd1` 和 `wd` 是一致的，具备写优先

同时 `rd0` 是延后了一个周期观察到数据，可以看到是正常的

![image-20220327232046353](report/image-20220327232046353.png)

后续再次从 0 开始读内存值，保持了一开始写的值：

![image-20220327232257870](report/image-20220327232257870.png)

### RAM IP 核的功能仿真和对比

此部分要求分别使用分布式和块式的 RAM IP 核，进行功能仿真，并对比

- 分布式和块式存储器的读操作
- 块式存储器写操作优先和读操作优先

这里我们选择一个模块里例化三个 ip 核，并对该模块仿真

例化过程实验文档已经给出，故这里不再展示

模块代码：

```verilog
module ram_ip(
  input clk,
  input we,
  input [7:0] a,
  input [15:0] d,
  output [15:0] spo,
  input ena,
  output [15:0] spo1,
  output [15:0] spo2);
    
  dist_mem_single dist (
    .a(a),      // input wire [7 : 0] a
    .d(d),      // input wire [15 : 0] d
    .clk(clk),  // input wire clk
    .we(we),    // input wire we
    .spo(spo)   // output wire [15 : 0] spo
  );
  
  blk_single_write_first blk_write (
    .clka(clk),   // input wire clka
    .ena(ena),    // input wire ena
    .wea(we),     // input wire [0 : 0] wea
    .addra(a),    // input wire [7 : 0] addra
    .dina(d),     // input wire [15 : 0] dina
    .douta(spo1)  // output wire [15 : 0] douta
  );
  
  blk_mem_read_first blk_read (
    .clka(clk),    // input wire clka
    .ena(ena),     // input wire ena
    .wea(we),      // input wire [0 : 0] wea
    .addra(a),     // input wire [7 : 0] addra
    .dina(d),      // input wire [15 : 0] dina
    .douta(spo2)   // output wire [15 : 0] douta
  );
endmodule
```

下面编写仿真文件：

```verilog
module ram_ip_sim();

reg clk, we;
reg [7:0] a;       // address
reg ena;           // only for block mem
reg [15:0] d;      // data

wire [15:0] spo;   // dis
wire [15:0] spo1;  // blk_write_first
wire [15:0] spo2;  // blk_read_first

ram_ip test_top (.clk(clk), .we(we), .a(a), .ena(ena), .d(d), .spo(spo), .spo1(spo1), .spo2(spo2));

// 时钟，周期为 10
initial begin
    clk = 1;
    forever begin
        #5 clk = ~clk;
    end
end

// 地址，每两个周期更新一次
initial begin
    a = 0;
    #13 a = 1;
    forever begin
        #20 a = a + 1;
    end
end

// 地址使能，这里我们需要观察储存器情况，所以直接置换 1
initial begin
    ena = 1;
end

// 16 位数据
initial begin
    d = $urandom();
    #13 d = $urandom();
    forever begin
        #20 d = $urandom();
    end
end

// 写使能，两个周期一组，取第一组的上升沿
initial begin
    we = 1;
    #3 we = 0;
    forever begin
        #10 we = ~we;
    end
end

endmodule
```

下面直接对波形文件进行分析：

`spo, spo1, spo2` 分别是分布式，写优先块和读优先块：

![image-20220328172929359](report/image-20220328172929359.png)

可以看到：

- 对于分布式 ram，spo 是异步读取的，相当于组合逻辑，而写入是同步的，写入之后，读取 spo 得到的是刚刚写入的值（相当于原先值直接被同步的写入操作覆盖了）

- 块式存储器的 spo 是同步读取，如果当前上升沿有写使能，那么
  - 写操作优先读出的是正在写的数据
  - 读操作优先读出的是对应地址之前的数据
  
  并且二者的值都会稳定持续一个周期

### 排序电路

#### 数据通路及控制器设计

本部分要求：

- 采用分布式双端口存储器保存数据，例化时可以初始化数据
- 利用 chk 查看数据，数码管显示存储器的地址和数据
- 利用 x、del、addr、data 设置地址和修改数据
- run 启动排序, 同时启动时钟计数 cnt，busy 置 1
- 排序时不能人工查看和修改数据
- 排序结束后停止计数，busy 清零

![image-20220328174008444](report/image-20220328174008444.png)

## 总结与思考

- 本次实验增强了我的数据通路设计和代码编写及调试能力
- 本次实验难易程度较大，主要是与上一个实验之间的跨度不太合理
- 本次实验文档说明不够清晰，希望可以改进
- 本次实验任务量设置较大
