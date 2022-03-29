`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date: 2022/03/28 14:35:07
// Design Name:
// Module Name: seg_display
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


module seg_display (
        input clk,              // 时钟
        input [7:0] addr,       // 待显示地址
        input [15:0] data,      // 待显示数据
        output reg [7:0] an,    // [7:6] 地址，[3:0] 数据
        output [6:0] seg        // 七段数码管
    );

    reg [3:0] num;              // 当前时刻需要输出的数字
    seg_decoder decoder(.hex_num(num), .seg(seg));

    reg [20:0] show_count;       // 计数器分时复用, 高三位取 0~5，映射六个状态
    always@(posedge clk) begin
        if (show_count >= 21'h17ffff)
            show_count <= 21'b0;
        else
            show_count <= show_count + 1;
    end

    always @(*) begin
        an = 8'hff;
        case (show_count[20:18])
            3'b000: begin
                an[0] = 0;
                num = data[3:0];
            end
            3'b001: begin
                an[1] = 0;
                num = data[7:4];
            end
            3'b010: begin
                an[2] = 0;
                num = data[11:8];
            end
            3'b011: begin
                an[3] = 0;
                num = data[15:12];
            end
            3'b100: begin
                an[6] = 0;
                num = addr[3:0];
            end
            3'b101: begin
                an[7] = 0;
                num = addr[7:4];
            end
            default: begin
                an[0] = 0;
                num = data[3:0];
            end
        endcase
    end
endmodule  //seg_display
