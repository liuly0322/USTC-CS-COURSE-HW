module _top(input CLK,
           input CPU_RESETN,
           input BTNC,
           input [15:0] SW,
           output reg [15:0] LED);
    reg [5:0] a;
    reg [5:0] b;
    reg [2:0] s;
    
    wire en_edge;
    btn_edge get_edge(.clk(CLK), .button(BTNC), .button_edge(en_edge));
    
    always @(posedge CLK) begin
        if (!CPU_RESETN) begin
            a <= 6'b0;
            b <= 6'b0;
            s <= 3'b0;
        end
        else begin
            if (en_edge) begin
                s <= SW[15:13];
                a <= SW[11:6];
                b <= SW[5:0];
            end
        end
    end
    
    wire [5:0] out_wire_y;
    wire [2:0] out_wire_f;
    alu #(.WIDTH(6)) alu1(.a(a), .b(b), .s(s), .y(out_wire_y), .f(out_wire_f));

    always @(posedge CLK) begin
        if (!CPU_RESETN) begin
            LED[5:0] <= 6'b0;
            LED[15:13] <= 3'b0;
        end
        else begin
            LED[5:0] <= out_wire_y;
            LED[15:13] <= out_wire_f;
        end
        
    end

endmodule
