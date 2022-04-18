module register_file #(
        parameter AW = 5,
        parameter DW = 32)
    (input  clk,
     input [AW-1: 0] ra0,   // eû10@
     input [AW-1: 0] ra1,   // eû20@
     input [AW-1: 0] ra_debug,
     output [DW-1:0] rd0,   // eû1<
     output [DW-1:0] rd1,   // eû2<
     output [DW-1:0] rd_debug,
     input [AW-1: 0] wa,    // e0@
     input [DW-1: 0] wd,    // e<
     input we);             // eý

    reg [DW-1: 0] rf[0: (1<<AW)-1];
    assign rd0 = (ra0 != 0) ? rf[ra0]: 0;
    assign rd1 = (ra1 != 0) ? rf[ra1]: 0;
    assign rd_debug = (ra_debug != 0)? rf[ra_debug] : 0;

    always @(posedge clk)
        if (we) rf[wa] <= wd;
endmodule
