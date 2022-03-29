`timescale 1ns / 1ps

module sort_sim (
        input  clk,         // clk
        input  rstn,        // cpu_rstn
        input  run,		    // btnd, 启动排序
        output sort_end
    );
    wire [7:0] dpra;
    wire [15:0] spo, dpo;          // 这些不存在竞争问题
    wire [7:0] a;
    wire [15:0] d;
    wire we;
    dist_mem mem (
                 .a(a),        // read/write address input
                 .d(d),        // write data input
                 .dpra(dpra),  // read address input
                 .clk(clk),    // input wire clk
                 .we(we),      // input wire we
                 .spo(spo),    // output wire [15 : 0] spo
                 .dpo(dpo)    // output wire [15 : 0] dpo
             );
    sort_module s_m (.clk(clk), .rstn(rstn), .run(run), .spo(spo), .dpo(dpo), .a(a), .d(d), .we(we), .dpra(dpra), .finish(sort_end));
endmodule
