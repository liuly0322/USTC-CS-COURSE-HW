module bytestohex(input [7:0] bytes,
                  output reg [3:0] address);
// 从 1 个字节 ascii 转换成 4 位的地址，例如 f 转换成 1111
always @(*) begin
    case (bytes)
        8'h30: address   = 4'h0;
        8'h31: address   = 4'h1;
        8'h32: address   = 4'h2;
        8'h33: address   = 4'h3;
        8'h34: address   = 4'h4;
        8'h35: address   = 4'h5;
        8'h36: address   = 4'h6;
        8'h37: address   = 4'h7;
        8'h38: address   = 4'h8;
        8'h39: address   = 4'h9;
        8'h61: address   = 4'ha;
        8'h62: address   = 4'hb;
        8'h63: address   = 4'hc;
        8'h64: address   = 4'hd;
        8'h65: address   = 4'he;
        8'h66: address   = 4'hf;
        default: address = 4'h0;
    endcase
end
endmodule
