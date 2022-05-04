module top_cpu(
        input clk,              // clk100mhz
        input rstn,             // cpu_resetn

        input step,             // btnu
        input cont,             // btnd
        input chk,              // btnr
        input data,             // btnc
        input del,              // btnl
        input [15 : 0] x,       // sw15-0

        output stop,            // led16r
        output [15 : 0] led,    // led15-0
        output [7 : 0] an,      // an7-0
        output [6 : 0] seg,     // ca-cg
        output [2 : 0] seg_sel  // led17
    );
    wire clk_cpu;       // cpu's clk
    wire rst_cpu;       // cpu's rst

    // IO_BUS
    wire [7 : 0] io_addr;
    wire [31 : 0] io_dout;
    wire io_we;
    wire io_rd;
    wire [31 : 0] io_din;

    // Debug_BUS
    wire [31 : 0] pc;
    wire [15 : 0] chk_addr;
    wire [31 : 0] chk_data;

    pdu PDU (.clk(clk), .rstn(rstn),
             .step(step), .cont(cont), .chk(chk), .data(data), .del(del), .x(x),
             .stop(stop), .led(led), .an(an), .seg(seg), .seg_sel(seg_sel),
             .clk_cpu(clk_cpu), .rst_cpu(rst_cpu),
             .io_addr(io_addr), .io_dout(io_dout), .io_we(io_we), .io_rd(io_rd), .io_din(io_din),
             .pc(pc), .chk_addr(chk_addr), .chk_data(chk_data));

    cpu CPU (.clk(clk_cpu), .rstn(~rst_cpu),
             .io_addr(io_addr), .io_dout(io_dout), .io_we(io_we), .io_rd(io_rd), .io_din(io_din),
             .pc(pc), .chk_addr(chk_addr), .chk_data(chk_data));
endmodule
