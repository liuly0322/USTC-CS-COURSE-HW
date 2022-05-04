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


wire [31:0]	ir_IF;
wire [31:0]	pc_IF;
wire [31:0]	pc_4_IF;
wire [31: 0] reg_data_debug;
wire [31: 0] pc_ID;
wire [31: 0] pc_4_ID;
wire [31: 0] rd1_ID;
wire [31: 0] rd2_ID;
wire [31: 0] imm_ID;
wire [2: 0]  ctrl_branch_ID;
wire ctrl_jump_ID;
wire ctrl_mem_r_ID;
wire ctrl_mem_w_ID;
wire [1:0] ctrl_wb_reg_src_ID;
wire [2:0] ctrl_alu_op_ID;
wire ctrl_pc_add_src_ID;
wire ctrl_alu_src1_ID;
wire ctrl_alu_src2_ID;
wire ctrl_reg_write_ID;
wire          ctrl_reg_write_EX;
wire [1: 0]   ctrl_wb_reg_src_EX;
wire          ctrl_mem_r_EX;
wire [31: 0]  alu_out_EX;
wire         pc_branch_EX;
wire         pc_jump_EX;
wire [31: 0]  pc_nxt_EX;
wire [31: 0]  rd2_EX;
wire [31: 0]  pc_4_EX;
wire          ctrl_mem_w_EX;
wire [31: 0]  pc_4_MEM;
wire [31: 0]  alu_out_MEM;
wire [31: 0]  mdr;
wire [31: 0]  mdr_MEM;
wire [1: 0]   ctrl_wb_reg_src_MEM;
wire [31: 0] mem_data_debug;
wire         reg_wb_en;
wire [4: 0]  reg_wb_addr;
wire [4: 0]  reg_wb_addr_ID;
wire [4: 0]  reg_wb_addr_EX;
wire [31: 0] reg_wb_data;
wire [31: 0] alu_out;
wire stall_IF;
wire flush_ID;

hazard u_hazard(
    .rstn               ( rstn          ),
    .ctrl_mem_r_ID      ( ctrl_mem_r_ID ),
    .pc_branch_EX       ( pc_branch_EX  ), 
    .pc_jump_EX         ( pc_jump_EX    ),
    .stall_IF           ( stall_IF      ),
    .flush_ID           ( flush_ID      )
);

IF u_IF(
	.clk          		( clk          		),
    .stall_IF           ( stall_IF          ),
	.pc_nxt_EX    		( pc_nxt_EX    		),
	.pc_branch_EX 		( pc_branch_EX 		),
	.pc_jump_EX   		( pc_jump_EX   		),
	.ir_IF        		( ir_IF        		),
	.pc_IF        		( pc_IF        		),
	.pc_4_IF      		( pc_4_IF      		)
);

wire [4:0] reg_addr_debug = chk_addr[4: 0];

ID u_ID(
    .clk                ( clk               ),
    .flush_ID           ( flush_ID          ),
    .ctrl_reg_write_EX  ( ctrl_reg_write_EX ),
    .ctrl_wb_reg_src_EX ( ctrl_wb_reg_src_EX),
    .alu_out_EX         ( alu_out_EX        ),
    .pc_4_EX            ( pc_4_EX           ),
    .reg_wb_addr_EX     ( reg_wb_addr_EX    ),
    .reg_addr_debug     ( reg_addr_debug    ),
    .reg_data_debug     ( reg_data_debug    ),
    .reg_wb_en          ( reg_wb_en         ),
    .reg_wb_addr        ( reg_wb_addr       ),
    .reg_wb_data        ( reg_wb_data       ),
    .alu_out            ( alu_out           ),
    .mdr                ( mdr               ),
    .pc_IF              ( pc_IF             ),
    .pc_4_IF            ( pc_4_IF           ),
    .ir_IF              ( ir_IF             ),
    .pc_ID              ( pc_ID             ),
    .pc_4_ID            ( pc_4_ID           ),
    .rd1_ID             ( rd1_ID            ),
    .rd2_ID             ( rd2_ID            ),
    .imm_ID             ( imm_ID            ),
    .reg_wb_addr_ID     ( reg_wb_addr_ID    ),
    .ctrl_branch_ID     ( ctrl_branch_ID    ),
    .ctrl_jump_ID       ( ctrl_jump_ID      ),
    .ctrl_mem_r_ID      ( ctrl_mem_r_ID     ),
    .ctrl_mem_w_ID      ( ctrl_mem_w_ID     ),
    .ctrl_wb_reg_src_ID ( ctrl_wb_reg_src_ID),
    .ctrl_alu_op_ID     ( ctrl_alu_op_ID    ),
    .ctrl_pc_add_src_ID ( ctrl_pc_add_src_ID),
    .ctrl_alu_src1_ID   ( ctrl_alu_src1_ID  ),
    .ctrl_alu_src2_ID   ( ctrl_alu_src2_ID  ),
    .ctrl_reg_write_ID  ( ctrl_reg_write_ID )
);

EX u_EX(
    .clk                ( clk               ),
    .alu_out            ( alu_out           ),
    .ctrl_alu_op_ID     ( ctrl_alu_op_ID    ),
    .ctrl_alu_src1_ID   ( ctrl_alu_src1_ID  ),
    .ctrl_alu_src2_ID   ( ctrl_alu_src2_ID  ),
    .ctrl_pc_add_src_ID ( ctrl_pc_add_src_ID),
    .ctrl_branch_ID     ( ctrl_branch_ID    ),
    .ctrl_jump_ID       ( ctrl_jump_ID      ),
    .imm_ID             ( imm_ID            ),
    .rd1_ID             ( rd1_ID            ),
    .pc_ID              ( pc_ID             ),
    .pc_4_ID            ( pc_4_ID           ),
    .rd2_ID             ( rd2_ID            ),
    .ctrl_reg_write_ID  ( ctrl_reg_write_ID ),
    .reg_wb_addr_ID     ( reg_wb_addr_ID    ),
    .reg_wb_addr_EX     ( reg_wb_addr_EX    ),
    .ctrl_wb_reg_src_ID ( ctrl_wb_reg_src_ID),
    .ctrl_mem_r_ID      ( ctrl_mem_r_ID     ),
    .ctrl_mem_w_ID      ( ctrl_mem_w_ID     ),
    .ctrl_reg_write_EX  ( ctrl_reg_write_EX ),
    .ctrl_wb_reg_src_EX ( ctrl_wb_reg_src_EX),
    .ctrl_mem_r_EX      ( ctrl_mem_r_EX     ),
    .alu_out_EX         ( alu_out_EX        ),
    .pc_branch_EX       ( pc_branch_EX      ),
    .pc_jump_EX         ( pc_jump_EX        ),
    .pc_nxt_EX          ( pc_nxt_EX         ),
    .rd2_EX             ( rd2_EX            ),
    .pc_4_EX            ( pc_4_EX           ),
    .ctrl_mem_w_EX      ( ctrl_mem_w_EX     )
);

wire [9: 0] mem_addr_debug = chk_addr[9:0];

MEM u_MEM(
    .mem_addr_debug     ( mem_addr_debug    ),
    .io_din             ( io_din            ),
    .mem_data_debug     ( mem_data_debug    ),
    .io_addr            ( io_addr           ),
    .io_dout            ( io_dout           ),
    .io_we              ( io_we             ),
    .io_rd              ( io_rd             ),
    .clk                ( clk               ),
    .pc_4_EX            ( pc_4_EX           ),
    .alu_out_EX         ( alu_out_EX        ),
    .rd2_EX             ( rd2_EX            ),
    .ctrl_mem_r_EX      ( ctrl_mem_r_EX     ),
    .ctrl_mem_w_EX      ( ctrl_mem_w_EX     ),
    .ctrl_reg_write_EX  ( ctrl_reg_write_EX ),
    .ctrl_wb_reg_src_EX ( ctrl_wb_reg_src_EX),
    .pc_4_MEM           ( pc_4_MEM          ),
    .alu_out_MEM        ( alu_out_MEM       ),
    .mdr                ( mdr               ),
    .mdr_MEM            ( mdr_MEM           ),
    .reg_wb_addr_EX     ( reg_wb_addr_EX    ),
    .reg_wb_en          ( reg_wb_en         ),
    .reg_wb_addr        ( reg_wb_addr       ),
    .ctrl_wb_reg_src_MEM( ctrl_wb_reg_src_MEM)
);

WB u_WB(
    .pc_4_MEM           ( pc_4_MEM          ),
    .alu_out_MEM        ( alu_out_MEM       ),
    .mdr_MEM            ( mdr_MEM           ),
    .ctrl_wb_reg_src_MEM( ctrl_wb_reg_src_MEM),
    .reg_wb_data        ( reg_wb_data       )
);

reg [31:0] debug_data;
assign chk_data = debug_data;
assign pc       = pc_4_IF;
always @(*) begin
    if (chk_addr[15:12] == 1) begin        // 查看寄存器值
        debug_data = reg_data_debug;
    end
    else if (chk_addr[15:12] == 2) begin   // 数据主存
        debug_data = mem_data_debug;
    end
    else begin                             // 查看 pcs
        case (chk_addr[3:0])
            4'h1:
                debug_data = pc_IF;
            4'h2:
                debug_data = pc_ID;
            4'h3:
                debug_data = ir_IF;
            4'h4:
                debug_data = {12'h0, ctrl_alu_op_ID, ctrl_alu_src1_ID, ctrl_alu_src2_ID,
                            ctrl_branch_ID, ctrl_jump_ID, ctrl_mem_r_EX, ctrl_mem_w_EX,
                            ctrl_pc_add_src_ID, ctrl_reg_write_EX, ctrl_reg_write_ID,
                            ctrl_wb_reg_src_EX, ctrl_wb_reg_src_ID, ctrl_wb_reg_src_MEM};
            4'h5:
                debug_data = pc_4_EX;
            4'h6:
                debug_data = alu_out_EX;
            4'h7:
                debug_data = rd2_EX;
            4'h8:
                debug_data = imm_ID;
            4'ha:
                debug_data = alu_out_MEM;
            4'hb:
                debug_data = mdr_MEM;
            4'hc:
                debug_data = {31'h0, reg_wb_en};
            4'hd:
                debug_data = reg_wb_data;
            default:
                debug_data = pc_nxt_EX;
        endcase
    end
end

endmodule
