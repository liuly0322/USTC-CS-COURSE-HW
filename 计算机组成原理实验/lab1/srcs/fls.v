module fls (input clk,
                input rstn,
                input en,
                input [15: 0] d,
                output reg [15: 0] f);

    reg [15: 0] a;
    reg [15: 0] b;

    parameter S_LOAD_0 = 2'b00;		// 初始状态，等待载入第一个数
    parameter S_LOAD_1 = 2'b01;     // 等待载入第二个数
    parameter S_WAIT   = 2'b10;     // 等待使能

    reg[1:0] curr_state;
    reg[1:0] next_state;

    // 状态机状态转换部分
    always @(posedge clk) begin
        curr_state <= (~rstn)? S_LOAD_0 : next_state;
    end

    // 状态机组合逻辑部分
    always @(*) begin
        case (curr_state)
            S_LOAD_0:
                next_state = en? S_LOAD_1 : S_LOAD_0;
            S_LOAD_1:
                next_state = en? S_WAIT : S_LOAD_1;
            S_WAIT:
                next_state = S_WAIT;
            default:
                next_state = S_LOAD_0;
        endcase
    end

    // 输出逻辑
    always @(*) begin
        case (curr_state)
            S_LOAD_1 :
                f = a;
            S_WAIT:
                f = b;
            default:
                f = 0;
        endcase
    end

    // 载入逻辑
    /* verilator lint_off CASEINCOMPLETE */
    always @(posedge clk) begin
        if (en) begin
            case (curr_state)
                S_LOAD_0:
                    a <= d;
                S_LOAD_1:
                    b <= d;
                S_WAIT: begin
                    a <= b;
                    b <= a + b;
                end
            endcase
        end
    end

endmodule
