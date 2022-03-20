module top(input CLK,
           input CPU_RESETN,
           input BTNC,
           input [15:0] SW,
           output [15:0] LED);
    
    // 去毛刺取边沿
    wire en_edge;
    btn_edge get_edge(.clk(CLK), .button(BTNC), .button_edge(en_edge));
    
    // 连接 fls 模块
    fls fls1(.clk(CLK), .rstn(CPU_RESETN), .en(en_edge), .d(SW), .f(LED));
endmodule
