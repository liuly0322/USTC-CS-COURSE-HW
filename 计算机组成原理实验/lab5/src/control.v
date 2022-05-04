module control (
     input [31:0] ir,
     output [2:0] control_branch,
     output control_jump,
     output control_mem_read,
     output control_mem_write,
     output [1:0] control_wb_reg_src,
     output [2:0] control_alu_op,
     output control_pc_add_src,
     output control_alu_src1,
     output control_alu_src2,
     output control_reg_write);

wire [6:0] opcd = ir[6:0];

wire is_lw     = (opcd == 7'b0000011);
wire is_branch = (opcd == 7'b1100011);
wire is_sw     = (opcd == 7'b0100011);
wire is_jal    = (opcd == 7'b1101111);
wire is_jalr   = (opcd == 7'b1100111);
wire is_auipc  = (opcd == 7'b0010111);
wire is_sub    = (opcd == 7'b0110011);
wire is_addi   = (opcd == 7'b0010011);

assign control_branch     = {is_branch, ir[14], ir[12]};
assign control_jump       = is_jal | is_jalr;                       // 跳转指令
assign control_mem_read   = is_lw;                                  // 是否读数据主存
assign control_mem_write  = is_sw;                                  // 是否写数据主存
assign control_pc_add_src = is_jalr;                                // 是否 pc 从寄存器堆获取数据
assign control_alu_src1   = is_auipc;                               // auipc, alu 需要 pc 和 imm 运算
assign control_alu_src2   = is_auipc | is_addi | is_lw | is_sw;     // 立即数运算
assign control_reg_write  = ~(is_branch | is_sw);                   // 是否写回寄存器

reg [1:0] wb_signal;
assign control_wb_reg_src = wb_signal;
always@(*) begin
    wb_signal = 0;                  // 默认 0, 用 alu 运算结果
    if (is_lw) wb_signal = 2'b01;   // 访存
    else if (is_jal | is_jalr) begin 
        wb_signal = 2'b10;          // 用下一个 pc
    end
end

assign control_alu_op = ((is_sub & ir[30]) | is_branch)? 3'b0 : 3'b1;

endmodule
