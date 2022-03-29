`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date: 2022/03/28 15:04:11
// Design Name:
// Module Name: get_x
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


module get_x(
        input clk,
        input [15:0] sw,
        output x_pulse,
        output reg [3:0] x
    );

    reg [15:0] old_state;       // 上一个稳定的 16 位信号
    reg [15:0] prev_state;      // 上一个周期的 16 位信号
    reg [15:0] cnt;             // prev_state 如果不同，维持了多久

    // 当且仅当 cnt 大于一个定值之后，diff 状态并输出信号
    always @(posedge clk) begin
        if (prev_state == old_state || sw !== prev_state || cnt >= 16'h8000)
            cnt <= 0;
        else
            cnt <= cnt + 1;
    end

    always @(posedge clk) begin
        prev_state <= sw;
    end

    always @(posedge clk) begin
        if (cnt[15]) begin
            old_state <= prev_state;
        end
    end

    assign x_pulse = cnt[15];

    wire [15:0] diff;
    assign diff = old_state ^ prev_state;

    // 使用编码器，返回一个 16 进制数
    always @(*) begin
        case (diff)
            16'h0001:
                x = 4'h0;
            16'h0002:
                x = 4'h1;
            16'h0004:
                x = 4'h2;
            16'h0008:
                x = 4'h3;
            16'h0010:
                x = 4'h4;
            16'h0020:
                x = 4'h5;
            16'h0040:
                x = 4'h6;
            16'h0080:
                x = 4'h7;
            16'h0100:
                x = 4'h8;
            16'h0200:
                x = 4'h9;
            16'h0400:
                x = 4'ha;
            16'h0800:
                x = 4'hb;
            16'h1000:
                x = 4'hc;
            16'h2000:
                x = 4'hd;
            16'h4000:
                x = 4'he;
            16'h8000:
                x = 4'hf;
            default:
                x = 4'h0;
        endcase
    end

endmodule
