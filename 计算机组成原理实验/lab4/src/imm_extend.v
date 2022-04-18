module imm_extend (
     input [31:0] ir,
     output reg [31:0] im_ext);

wire [6:0] opcode = ir[6:0];
wire is_branch = (opcode == 7'b1100011);
wire is_lw     = (opcode == 7'b0000011);
wire is_sw     = (opcode == 7'b0100011);
wire is_jal    = (opcode == 7'b1101111);
wire is_jalr   = (opcode == 7'b1100111);
wire is_auipc  = (opcode == 7'b0010111);
wire is_addi   = (opcode == 7'b0010011);

// addi, auipc, jal, jalr, beq, blt, lw, sw
wire [31:0] imm12_i = {{20{ir[31]}}, ir[31:20]};
wire [31:0] imm12_r = {{20{ir[31]}}, ir[31:25], ir[11:7]};
wire [31:0] imm13_b = {{20{ir[31]}}, ir[7], ir[30:25], ir[11:8], 1'b0};
wire [31:0] imm21_j = {{12{ir[31]}}, ir[19:12], ir[20], ir[30:21], 1'b0};
wire [31:0] imm32_u = {ir[31:12], 12'b0};

always @(*) begin
    im_ext = 0;
    if (is_addi | is_lw | is_jalr) im_ext = imm12_i;
    else if (is_sw)                im_ext = imm12_r;
    else if (is_branch)            im_ext = imm13_b;
    else if (is_jal)               im_ext = imm21_j;
    else if (is_auipc)             im_ext = imm32_u;
end
endmodule
