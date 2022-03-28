`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
//
// Create Date: 2022/03/27 13:52:35
// Design Name:
// Module Name: register_file
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


module register_file #(
        parameter AW = 5,
        parameter DW = 32)
    (input  clk,
     input [AW-1: 0] ra0,   // 异步读1地址
     input [AW-1: 0] ra1,   // 异步读2地址
     output [DW-1:0] rd0,   // 异步读1值
     output [DW-1:0] rd1,   // 异步读2值
     input [AW-1: 0] wa,    // 同步写地址
     input [DW-1: 0] wd,    // 同步写值
     input we);             // 同步写使能

    reg [DW-1: 0] rf[0: (1<<AW)-1];
    assign rd0 = (ra0 == wa) && we ? wd : rf[ra0];
    assign rd1 = (ra1 == wa) && we ? wd : rf[ra1];

    always @(*) begin
        rf[0] = 0;
    end

    always @(posedge clk)
        if (wa !== 0 && we)
            rf[wa] <= wd;
endmodule
