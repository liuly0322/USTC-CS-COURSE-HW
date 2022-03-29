`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date: 2022/03/29 01:47:59
// Design Name:
// Module Name: io
// Project Name:
// Target Devices:
// Tool Versions:
// Description:
//
// Dependencies:
//
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
//
//////////////////////////////////////////////////////////////////////////////////


module io (
        input  clk,         // clk
        input  rstn,        // cpu_rstn
        input [15:0]  sw,	// sw, 输入一位数字
        input  del,		    // btnl, 删除一位数字
        input  set_addr,	// btnu, 保存地址
        input  set_data,	// btnc, 保存数据
        input  chk,		    // btnr, 切换到下一个地址
        input  run,		    // btnd, 启动排序
        input  sort_end,    // 代表排序结束的脉冲信号，由 S2 转 S0
        input  [15:0] spo,  // 输入值
        output run_IO,      // 脉冲，代表有 run 信号输入
        output [7:0]  an,	// 数码管显示存储器地址和数据
        output [6:0]  seg,  // 输出到数码管
        output busy,
        output reg [15:0] cnt,
        output reg [7:0] a, // 写入地址
        output reg [15:0] d,// 写入数据
        output we           // 使能
    );
    parameter S_CHK     = 2'b00;		// 初始状态，查看数据
    parameter S_INPUT   = 2'b01;		// 输入数据状态
    parameter S_WAIT    = 2'b10;		// 等待排序结束状态

    reg [1:0] cur_state, nxt_state;

    // 处理 sw，处理成一个脉冲（用于状态转移）和一个十六进制数（用于读取）
    wire x_pulse;
    wire [3:0] x_data;
    get_x get (.clk(clk), .sw(sw), .x_pulse(x_pulse), .x(x_data));

    // 处理各个按钮，统一去抖动去边沿
    wire del_IO, set_addr_IO, set_data_IO, chk_IO;
    btn_edge del_edge (clk, del, del_IO);
    btn_edge set_addr_edge (clk, set_addr, set_addr_IO);
    btn_edge set_data_edge (clk, set_data, set_data_IO);
    btn_edge chk_edge (clk, chk, chk_IO);
    btn_edge run_edge (clk, run, run_IO);

    // 输出数据
    wire [15:0] data = (cur_state == S_INPUT? d : spo);
    seg_display out_display (.clk(clk), .addr(a), .data(data), .an(an), .seg(seg));

    // 处理 d
    wire rst = (!rstn || set_addr_IO || set_data_IO || chk_IO);
    always @(posedge clk) begin
        if (rst)
            d <= 16'b0;
        else if (x_pulse)   // 左移
            d <= { d[11:0], x_data };
        else if (del_IO)    // 右移
            d <= { 4'b0, d[15:4] };
    end

    // 处理 a
    always @(posedge clk) begin
        if (!rstn)
            a <= 8'b0;
        else if (chk_IO || set_data_IO)
            a <= a + 1;
        else if (set_addr_IO)
            a <= d[7:0];
    end

    // 处理 we，当前仅当 set_data
    assign we = set_data_IO;

    // 内部 busy, cnt 信号，输出到 led
    assign busy = cur_state == S_WAIT;
    always @(posedge clk) begin
        if (run_IO)
            cnt <= 16'b0;
        else if (cur_state == S_WAIT)
            cnt <= cnt + 1;
    end

    // 状态机
    always @(posedge clk) begin
        if (!rstn)
            cur_state <= S_CHK;
        else
            cur_state <= nxt_state;
    end

    always @(*) begin
        case (cur_state)
            S_INPUT: begin
                nxt_state = (set_addr_IO || set_data_IO)? S_CHK : S_INPUT;
            end
            S_WAIT: begin
                nxt_state = sort_end ? S_CHK : S_WAIT;
            end
            default: begin  // S_CHK
                if (x_pulse)
                    nxt_state = S_INPUT;
                else if (run_IO)
                    nxt_state = S_WAIT;
                else
                    nxt_state = S_CHK;
            end
        endcase
    end
endmodule
