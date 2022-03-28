`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/03/28 07:08:24
// Design Name: 
// Module Name: ram_ip
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: In this file, you can use some ip core and
//              write testbench for this moudule
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


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
