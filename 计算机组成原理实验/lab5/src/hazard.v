module hazard(
           input rstn,
           input pc_branch_EX, 
           input pc_jump_EX,
           input ctrl_mem_r_ID,
           output stall_IF,
           output flush_ID
       );

assign stall_IF = ctrl_mem_r_ID;
assign flush_ID = pc_branch_EX | pc_jump_EX | ctrl_mem_r_ID;

endmodule
