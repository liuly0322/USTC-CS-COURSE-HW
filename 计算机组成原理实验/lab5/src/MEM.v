module MEM(
    input  [9: 0]       mem_addr_debug,
    input  [31: 0]      io_din,
    output [31: 0]      mem_data_debug,
    output [7:0]        io_addr,	    // 外设地址
    output [31:0]       io_dout,	    // 向外设输出的数据
    output              io_we,		    // 向外设输出数据时的写使能信号
    output              io_rd,		    // 从外设输入数据时的读使能信号
    input               clk,
    input [31: 0]       pc_4_EX,
    input [31: 0]       alu_out_EX,
    input [31: 0]       rd2_EX,
    input [4: 0]        reg_wb_addr_EX,
    input               ctrl_mem_r_EX,
    input               ctrl_mem_w_EX,
    input               ctrl_reg_write_EX,
    input [1: 0]        ctrl_wb_reg_src_EX,
    output reg [31: 0]  pc_4_MEM,
    output reg [31: 0]  alu_out_MEM,
    output [31: 0]      mdr,
    output reg [31: 0]  mdr_MEM,
    output reg          reg_wb_en,
    output reg [1: 0]   ctrl_wb_reg_src_MEM,
    output reg [4: 0]   reg_wb_addr
    );
    
    mem_wrapper data_mem (
                    .a(alu_out_EX),
                    .d(rd2_EX),
                    .dpra(mem_addr_debug),
                    .clk(clk),
                    .en(ctrl_mem_r_EX),
                    .we(ctrl_mem_w_EX),
                    .spo(mdr),
                    .dpo(mem_data_debug),
                    .io_addr(io_addr),
                    .io_din(io_din),
                    .io_dout(io_dout),
                    .io_rd(io_rd),
                    .io_we(io_we)
                );


always@(posedge clk) begin
    pc_4_MEM            <= pc_4_EX;
    alu_out_MEM         <= alu_out_EX;
    reg_wb_en           <= ctrl_reg_write_EX;
    ctrl_wb_reg_src_MEM <= ctrl_wb_reg_src_EX;
    mdr_MEM             <= mdr;
    reg_wb_addr         <= reg_wb_addr_EX;
end
endmodule
