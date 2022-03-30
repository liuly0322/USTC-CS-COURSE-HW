`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/03/30 11:50:41
// Design Name: 
// Module Name: extra_dis_sim
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


module extra_dis_sim(

    );

wire[11:0] a, dpra;
wire[15:0] d, spo, dpo;
reg clk;
wire we;

extra_dis your_instance_name (
  .a(a),        // input wire [11 : 0] a
  .d(d),        // input wire [15 : 0] d
  .dpra(dpra),  // input wire [11 : 0] dpra
  .clk(clk),    // input wire clk
  .we(we),      // input wire we
  .spo(spo),    // output wire [15 : 0] spo
  .dpo(dpo)    // output wire [15 : 0] dpo
);

reg run_IO;
wire _end;
reg rstn;

initial begin
    rstn = 0;
    #14 rstn = 1;
end

initial begin
    clk = 0;
    forever #5 clk = ~clk;
end

initial begin
    run_IO = 0;
    #23 run_IO = 1;
    #10 run_IO = 0;
end

extra_sort_module s_m (.clk(clk), .rstn(rstn), .run(run_IO), .spo(spo), .dpo(dpo), .a(a), .d(d), .we(we), .dpra(dpra), .finish(_end));
endmodule
