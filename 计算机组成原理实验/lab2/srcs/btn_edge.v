`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date: 2022/03/29 02:36:36
// Design Name:
// Module Name: btn_edge
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


module btn_edge(input clk,
                    input button,
                    output button_edge);

    reg [15:0] cnt;

    always@(posedge clk) begin
        if (button == 1'b0) begin
            cnt <= 0;
        end
        else begin
            if (cnt < 16'h8000) begin
                cnt <= cnt + 1'b1;
            end
        end
    end

    reg button_1, button_2;
    always @(posedge clk) begin
        button_1 <= cnt[15];
        button_2 <= button_1;
    end

    assign button_edge = button_1 & ~button_2;
endmodule
