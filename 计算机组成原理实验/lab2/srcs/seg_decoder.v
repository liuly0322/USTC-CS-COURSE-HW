module seg_decoder (
        input [3:0] hex_num,
        output [6:0] seg
    );

    reg[6:0] seg_out;
    assign seg = seg_out;
    always @(*) begin
        case(hex_num)
            4'b0000:
                seg_out = 7'b0000001; // "0"
            4'b0001:
                seg_out = 7'b1001111; // "1"
            4'b0010:
                seg_out = 7'b0010010; // "2"
            4'b0011:
                seg_out = 7'b0000110; // "3"
            4'b0100:
                seg_out = 7'b1001100; // "4"
            4'b0101:
                seg_out = 7'b0100100; // "5"
            4'b0110:
                seg_out = 7'b0100000; // "6"
            4'b0111:
                seg_out = 7'b0001111; // "7"
            4'b1000:
                seg_out = 7'b0000000; // "8"
            4'b1001:
                seg_out = 7'b0000100; // "9"
            4'b1010:
                seg_out = 7'b0001000; // "A"
            4'b1011:
                seg_out = 7'b1100000; // "B"
            4'b1100:
                seg_out = 7'b0110001; // "C"
            4'b1101:
                seg_out = 7'b1000010; // "D"
            4'b1110:
                seg_out = 7'b0110000; // "E"
            4'b1111:
                seg_out = 7'b0111000; // "F"
        endcase
    end

endmodule  //seg_decoder
