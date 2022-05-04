module IF(
    input clk, stall_IF,
    input [31: 0] pc_nxt_EX,
    input pc_branch_EX, pc_jump_EX,
    output reg [31: 0] ir_IF,
    output reg [31: 0] pc_IF,
    output [31: 0] pc_4_IF
    );

reg [31: 0] pc;
wire [31: 0] ir;
dist_ir ir_mem (.a(pc[9:2]), .d(0), .clk(clk), .we(0), .spo(ir));

assign pc_4_IF = pc_IF + 32'h4;

always@(posedge clk) begin
    if (!stall_IF) begin
        pc_IF <= pc;
        ir_IF <= ir;
    end
end

// pc(next) 是关于 pc_IF 的组合逻辑输出
always @(*) begin
    if (pc_branch_EX | pc_jump_EX)
        pc = pc_nxt_EX;
    else
        pc = pc_4_IF;
end

endmodule
