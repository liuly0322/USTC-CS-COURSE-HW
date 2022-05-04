
module ID(
    input clk,
    input          flush_ID,
    input          ctrl_reg_write_EX,
    input  [1: 0]  ctrl_wb_reg_src_EX,
    input  [31: 0] pc_4_EX,
    input  [4: 0]  reg_wb_addr_EX,
    input  [4: 0]  reg_addr_debug,
    output [31: 0] reg_data_debug,
    input          reg_wb_en,
    input [4: 0]   reg_wb_addr,
    input [31: 0]  reg_wb_data,
    input [31: 0]  pc_IF,
    input [31: 0]  pc_4_IF,
    input [31: 0]  ir_IF,
    input [31: 0]  alu_out,
    input [31: 0]  alu_out_EX,
    input [31: 0]  mdr,
    output reg [31: 0] pc_ID,
    output reg [31: 0] pc_4_ID,
    output reg [31: 0] rd1_ID,
    output reg [31: 0] rd2_ID,
    output reg [4: 0]  reg_wb_addr_ID, 
    output reg [31: 0] imm_ID,
    output reg [2: 0]  ctrl_branch_ID,
    output reg ctrl_jump_ID,
    output reg ctrl_mem_r_ID,
    output reg ctrl_mem_w_ID,
    output reg [1:0] ctrl_wb_reg_src_ID,
    output reg [2:0] ctrl_alu_op_ID,
    output reg ctrl_pc_add_src_ID,
    output reg ctrl_alu_src1_ID,
    output reg ctrl_alu_src2_ID,
    output reg ctrl_reg_write_ID
    );

    wire [2:0] control_branch;
    wire control_jump;
    wire control_mem_read;
    wire control_mem_write;
    wire [1:0] control_wb_reg_src;
    wire [2:0] control_alu_op;
    wire control_pc_add_src;
    wire control_alu_src1;
    wire control_alu_src2;
    wire control_reg_write;
    control control_unit (.ir(ir_IF), .control_branch(control_branch), .control_jump(control_jump),.control_mem_read(control_mem_read),
                          .control_mem_write(control_mem_write), .control_wb_reg_src(control_wb_reg_src), .control_alu_op(control_alu_op), .control_pc_add_src(control_pc_add_src),
                          .control_alu_src1(control_alu_src1), .control_alu_src2(control_alu_src2), .control_reg_write(control_reg_write));

    wire [4:0]  rs2  = ir_IF[24:20];
    wire [4:0]  rs1  = ir_IF[19:15];
    wire [31:0] rd1, rd2;
    register_file register (.clk(clk), .ra0(rs1), .ra1(rs2), .wa(reg_wb_addr),
                            .rd0(rd1), .rd1(rd2), .wd(reg_wb_data), .we(reg_wb_en),
                            .ra_debug(reg_addr_debug), .rd_debug(reg_data_debug));

    wire [31:0] imm_ext;
    imm_extend imm_extend_unit (.ir(ir_IF), .im_ext(imm_ext));

    reg [31:0] rd1_forward;
    always @(*) begin
        rd1_forward = rd1;
        if (ctrl_reg_write_EX && reg_wb_addr_EX == rs1) begin
            if (ctrl_wb_reg_src_EX == 2'b00)
                rd1_forward = alu_out_EX;
            else if (ctrl_wb_reg_src_EX == 2'b01)
                rd1_forward = mdr;
            else if (ctrl_wb_reg_src_EX == 2'b10)
                rd1_forward = pc_4_EX;
        end
        if (ctrl_reg_write_ID && reg_wb_addr_ID == rs1) begin
            if (ctrl_wb_reg_src_ID == 2'b00)
                rd1_forward = alu_out;
            else if (ctrl_wb_reg_src_ID == 2'b10)
                rd1_forward = pc_4_ID;
        end
    end
    reg [31:0] rd2_forward;
    always @(*) begin
        rd2_forward = rd2;
        if (ctrl_reg_write_EX && reg_wb_addr_EX == rs2) begin
            if (ctrl_wb_reg_src_EX == 2'b00)
                rd2_forward = alu_out_EX;
            else if (ctrl_wb_reg_src_EX == 2'b01)
                rd2_forward = mdr;
            else if (ctrl_wb_reg_src_EX == 2'b10)
                rd2_forward = pc_4_EX;
        end 
        if (ctrl_reg_write_ID && reg_wb_addr_ID == rs2) begin
            if (ctrl_wb_reg_src_ID == 2'b00)
                rd2_forward = alu_out;
            else if (ctrl_wb_reg_src_ID == 2'b10)
                rd2_forward = pc_4_ID;
        end 
    end

    always @(posedge clk) begin
        pc_ID   <= flush_ID? 0 :pc_IF;
        pc_4_ID <= flush_ID? 0 :pc_4_IF;
        rd1_ID  <= flush_ID? 0 :rd1_forward;
        rd2_ID  <= flush_ID? 0 :rd2_forward;
        imm_ID  <= flush_ID? 0 :imm_ext;
        reg_wb_addr_ID  <= flush_ID? 0 :ir_IF[11:7];
        ctrl_branch_ID  <= flush_ID? 0 :control_branch;
        ctrl_jump_ID    <= flush_ID? 0 :control_jump;
        ctrl_mem_r_ID   <= flush_ID? 0 :control_mem_read;
        ctrl_mem_w_ID   <= flush_ID? 0 :control_mem_write;
        ctrl_wb_reg_src_ID <= flush_ID? 0 :control_wb_reg_src;
        ctrl_alu_op_ID     <= flush_ID? 0 :control_alu_op;
        ctrl_pc_add_src_ID <= flush_ID? 0 :control_pc_add_src;
        ctrl_alu_src1_ID   <= flush_ID? 0 :control_alu_src1;
        ctrl_alu_src2_ID   <= flush_ID? 0 :control_alu_src2;
        ctrl_reg_write_ID  <= flush_ID? 0 :control_reg_write;
    end

endmodule
