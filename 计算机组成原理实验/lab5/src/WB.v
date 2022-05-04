module WB(
    input [31: 0]       pc_4_MEM,
    input [31: 0]       alu_out_MEM,
    input [31: 0]       mdr_MEM,
    input [1: 0]        ctrl_wb_reg_src_MEM,
    output reg [31: 0]  reg_wb_data
    );

    always @(*) begin
        case (ctrl_wb_reg_src_MEM)
            2'b01:
                reg_wb_data = mdr_MEM;
            2'b10:
                reg_wb_data = pc_4_MEM;
            default:
                reg_wb_data = alu_out_MEM;
        endcase
    end

endmodule
