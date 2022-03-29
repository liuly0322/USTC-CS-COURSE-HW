`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date: 2022/03/29 04:17:11
// Design Name:
// Module Name: sort_module
// Project Name:
// Target Devices:
// Tool Versions:
// Description: 排序用，接受状态开启信号，发送结束信号，并且操作 ram
//
// Dependencies:
//
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
//
//////////////////////////////////////////////////////////////////////////////////


module sort_module(
        input clk,
        input rstn,
        input run,
        input [15:0] spo,   // a 地址对应数据
        input [15:0] dpo,   // dpra 端口对应数据
        output [7:0] a,     // 可读可写的地址
        output reg [15:0] d,// 写入数据值
        output reg we,      // 写使能
        output [7:0] dpra,  // 只读的地址
        output finish       // 脉冲信号
    );

    reg [1:0] cur_state, nxt_state;
    reg [7:0] n, i1;
    wire [7:0] i2 = i1 + 1;
    assign a = i1;
    assign dpra = i2;
    reg sorted;

    reg [15:0] temp;        // 暂存还没写入的值
    reg ex_pulse;           // 交换产生一个脉冲，告诉 cmp 应该用 d 代替 spo
    wire [15:0] cmp_left = ex_pulse? d : spo;
    wire [15:0] cmp_right = dpo;

    parameter S_WAIT   = 2'b00;		// 等待数据写入，初始状态
    parameter S_CMP    = 2'b01;		// 主要的比较状态
    parameter S_LOOP   = 2'b10;     // 每次循环结束后判断
    parameter S_EXCH   = 2'b11;     // 交换状态

    wire LOOP_END;
    assign LOOP_END = (cur_state == S_LOOP);

    always @(posedge clk) begin
        if (!rstn || (cur_state == S_WAIT) || LOOP_END)
            sorted <= 1'b1;
        else if (cur_state == S_EXCH)
            sorted <= 1'b0;
    end

    always @(posedge clk) begin
        if (!rstn || (cur_state == S_WAIT))
            n <= 8'hff;
        else if (LOOP_END)
            n <= n - 1;
    end

    // i1 和 temp, 当需要交换的时候延缓一个周期，先更改 i1 处的值
    always @(posedge clk) begin
        if (!rstn || LOOP_END)
            i1 <= 8'b0;
        else if (cur_state == S_CMP) begin
            if (cmp_left <= cmp_right)
                i1 <= i1 + 1;
            else begin
                temp <= cmp_left;
            end
        end
        else if (cur_state == S_EXCH) begin
            i1 <= i1 + 1;
        end
    end

    // d 和 we
    always @(posedge clk) begin
        if (cur_state == S_CMP && cmp_left > cmp_right && i1 !== n) begin
            d <= cmp_right;
            we <= 1;
            ex_pulse <= 0;
        end
        else if (cur_state == S_EXCH) begin
            d <= temp;
            we <= 1;
            ex_pulse <= 1;
        end
        else begin
            d <= 16'b0;
            we <= 0;
            ex_pulse <= 0;
        end
    end

    // 排序结束依据
    assign finish = LOOP_END && sorted;

    // 状态机
    always @(posedge clk) begin
        cur_state <= nxt_state;
    end

    always @(*) begin
        case (cur_state)
            S_WAIT:
                nxt_state = run? S_CMP : S_WAIT;
            S_CMP: begin
                if (i1 == n)
                    nxt_state = S_LOOP;
                else if (cmp_left > cmp_right)
                    nxt_state = S_EXCH;
                else
                    nxt_state = S_CMP;
            end
            S_LOOP:
                nxt_state = sorted? S_WAIT : S_CMP;
            S_EXCH:
                nxt_state = S_CMP;
        endcase
    end

endmodule
