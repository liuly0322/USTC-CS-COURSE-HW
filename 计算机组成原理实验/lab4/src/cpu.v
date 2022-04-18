`timescale 1ns/1ps
module  cpu (
        input clk,
        input rstn,

        // IO_BUS
        output [7:0]  io_addr,	// 外设地址
        output [31:0]  io_dout,	// 向外设输出的数据
        output  io_we,		    // 向外设输出数据时的写使能信号
        output  io_rd,		    // 从外设输入数据时的读使能信号
        input [31:0]  io_din,	    // 来自外设输入的数据

        // Debug_BUS
        output [31:0] pc,      	// 当前执行指令地址
        input [15:0] chk_addr,	// 数据通路状态的编码地址
        output [31:0] chk_data    // 数据通路状态的数据
    );

    ///////////////////////////////////////////////
    // pc & ir
    ///////////////////////////////////////////////
    reg  [31:0] cur_pc;
    wire [31:0] nxt_pc;
    wire [31:0] pc_4 = cur_pc + 4;
    assign pc = cur_pc;
    always@(posedge clk) begin
        if (~rstn)
            cur_pc <= 32'h3000;
        else
            cur_pc <= nxt_pc;
    end

    wire [31:0] ir;
    dist_ir ir_mem (.a(cur_pc[9:2]), .d(0), .clk(clk), .we(0), .spo(ir));

    wire[4:0] rs2  = ir[24:20];
    wire[4:0] rs1  = ir[19:15];
    wire[4:0] rd   = ir[11:7];
    wire[6:0] opcd = ir[6:0];

    ///////////////////////////////////////////////
    // imm_extend
    ///////////////////////////////////////////////
    wire [31:0] im_ext;
    imm_extend imm_extend_unit (.ir(ir), .im_ext(im_ext));

    ///////////////////////////////////////////////
    // control unit
    ///////////////////////////////////////////////
    wire control_branch;
    wire control_jump;
    wire control_mem_read;
    wire control_mem_write;
    wire [1:0] control_wb_reg_src;
    wire [2:0] control_alu_op;
    wire control_pc_add_src;
    wire control_alu_src1;
    wire control_alu_src2;
    wire control_reg_write;
    control control_unit (.ir(opcd), .ir30(ir[30]), .control_branch(control_branch), .control_jump(control_jump),.control_mem_read(control_mem_read),
                          .control_mem_write(control_mem_write), .control_wb_reg_src(control_wb_reg_src), .control_alu_op(control_alu_op), .control_pc_add_src(control_pc_add_src),
                          .control_alu_src1(control_alu_src1), .control_alu_src2(control_alu_src2), .control_reg_write(control_reg_write));

    ///////////////////////////////////////////////
    // register file
    ///////////////////////////////////////////////
    wire [31:0] rd1, rd2, rf_debug_data;
    reg [31:0] write_register_data;
    wire [31:0] mem_out, alu_out;
    always @(*) begin
        case (control_wb_reg_src)
            2'b01:
                write_register_data = mem_out;
            2'b10:
                write_register_data = pc_4;
            default:
                write_register_data = alu_out;
        endcase
    end
    register_file register (.clk(clk), .ra0(rs1), .ra1(rs2), .wa(rd),
                            .rd0(rd1), .rd1(rd2), .wd(write_register_data), .we(control_reg_write),
                            .ra_debug(chk_addr[4:0]), .rd_debug(rf_debug_data));

    ///////////////////////////////////////////////
    // data memory
    ///////////////////////////////////////////////
    wire [31:0] mem_debug_out;

    mem_wrapper data_mem (
                    .a(alu_out),
                    .d(rd2),
                    .dpra(chk_addr[9:0]),
                    .clk(clk),
                    .en(control_mem_read),
                    .we(control_mem_write),
                    .spo(mem_out),
                    .dpo(mem_debug_out),
                    .io_addr(io_addr),
                    .io_din(io_din),
                    .io_dout(io_dout),
                    .io_rd(io_rd),
                    .io_we(io_we)
                );

    ///////////////////////////////////////////////
    // alu
    ///////////////////////////////////////////////
    wire [31:0] alu_in1 = control_alu_src1 ? cur_pc : rd1;
    wire [31:0] alu_in2 = control_alu_src2 ? im_ext : rd2;
    wire [2:0]  alu_f;
    alu alu32 (.a(alu_in1), .b(alu_in2), .s(control_alu_op), .y(alu_out), .f(alu_f));


    ///////////////////////////////////////////////
    // pc control
    ///////////////////////////////////////////////
    wire jump = (control_branch & ((ir[14] == 1)? alu_f[1] : alu_f[0])) | control_jump;
    wire [31:0] pc_add_src = control_pc_add_src? rd1: cur_pc;
    assign nxt_pc = jump? (pc_add_src + im_ext) : pc_4;

    ///////////////////////////////////////////////
    // debug signal
    ///////////////////////////////////////////////
    reg [31:0] debug_data;
    assign chk_data = debug_data;
    always @(*) begin
        if (chk_addr[15:12] == 1) begin        // 查看寄存器值
            debug_data = rf_debug_data;
        end
        else if (chk_addr[15:12] == 2) begin   // 数据主存
            debug_data = mem_debug_out;
        end
        else begin                             // 查看 pcs
            case (chk_addr[3:0])
                4'h1:
                    debug_data = pc;
                4'h2:
                    debug_data = ir;
                4'h3:
                    debug_data = {19'h0, control_jump, control_branch, control_mem_read, control_mem_write,
                                  control_wb_reg_src, control_alu_op, control_pc_add_src,
                                  control_alu_src1, control_alu_src2, control_reg_write};
                4'h4:
                    debug_data = rd1;
                4'h5:
                    debug_data = rd2;
                4'h6:
                    debug_data = im_ext;
                4'h7:
                    debug_data = alu_out;
                4'h8:
                    debug_data = mem_out;
                default:
                    debug_data = nxt_pc;
            endcase
        end
    end

endmodule
