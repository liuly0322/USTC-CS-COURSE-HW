module hextobytes(input [3:0] hex,
                  output reg [7:0] bytes);
// 16 进制转 ascii
always @(*) begin
    case (hex)
        4'h0: bytes = 8'h30;
        4'h1: bytes = 8'h31;
        4'h2: bytes = 8'h32;
        4'h3: bytes = 8'h33;
        4'h4: bytes = 8'h34;
        4'h5: bytes = 8'h35;
        4'h6: bytes = 8'h36;
        4'h7: bytes = 8'h37;
        4'h8: bytes = 8'h38;
        4'h9: bytes = 8'h39;
        4'ha: bytes = 8'h61;
        4'hb: bytes = 8'h62;
        4'hc: bytes = 8'h63;
        4'hd: bytes = 8'h64;
        4'he: bytes = 8'h65;
        4'hf: bytes = 8'h66;
        default: bytes = 8'h30;
    endcase
end
endmodule
