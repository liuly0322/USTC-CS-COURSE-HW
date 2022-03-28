`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/03/28 08:48:04
// Design Name: 
// Module Name: ram_ip_sim
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
