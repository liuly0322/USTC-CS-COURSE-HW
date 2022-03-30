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


module extra_block_sim(

    );

wire[11:0] a, dpra;
wire[15:0] d, spo, dpo;
reg clk;
wire we;

blk_mem_gen_0 your_instance_name (
  .clka(clk),    // input wire clka
  .ena(1),      // input wire ena
  .wea(we),      // input wire [0 : 0] wea
  .addra(a),  // input wire [11 : 0] addra
  .dina(d),    // input wire [15 : 0] dina
  .douta(spo),  // output wire [15 : 0] douta
  .clkb(clk),    // input wire clkb
  .enb(1),      // input wire enb
  .web(0),      // input wire [0 : 0] web
  .addrb(dpra),  // input wire [11 : 0] addrb
  .dinb(0),    // input wire [15 : 0] dinb
  .doutb(dpo)  // output wire [15 : 0] doutb
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
