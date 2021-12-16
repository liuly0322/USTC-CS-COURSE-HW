`include "rx.v"
`include "bytestohex.v"

module top(input clk,
           rst,
           input rx,
           output reg [2:0] hexplay_an,
           output reg [3:0] hexplay_data);
    
    // 状态机状态
    parameter S_INPUT   = 3'b000;		// 等待数据写入，可作为初始状态
    parameter S_PARSE   = 3'b001;		// 解析缓冲区指令
    parameter S_FETCH   = 3'b010;       // 取指令
    parameter S_OP      = 3'b011;       // 执行操作
    parameter S_COND    = 3'b101;       // 更新操作码
    
    // lc-3 指令
    parameter LC3_BR    = 4'b0000;
    parameter LC3_ADD   = 4'b0001;
    parameter LC3_AND   = 4'b0101;
    parameter LC3_NOT   = 4'b1001;
    parameter LC3_LD    = 4'b0010;
    parameter LC3_ST    = 4'b0011;
    parameter LC3_LDR   = 4'b0110;
    parameter LC3_STR   = 4'b0111;
    parameter LC3_LEA   = 4'b1110;
    parameter LC3_JSR   = 4'b0100;
    parameter LC3_JMP   = 4'b1100;
    parameter LC3_TRAP  = 4'b1111;

    reg [2:0] curr_state;
    reg [2:0] next_state;

    initial curr_state = 3'b000;
    
    // 内存部分
    reg [15:0] mem[127:0];
    
    // 寄存器
    reg [15:0] R[7:0];
    reg [7:0] pc;
    reg [15:0] ir;
    reg n,z,p;
    
    // 读端口 & buffer 区
    wire        [7:0]   rx_data;
    wire                rx_vld;
    rx                  rx_inst(
    .clk                (clk),
    .rst                (rst),
    .rx                 (rx),
    .rx_vld             (rx_vld),
    .rx_data            (rx_data)
    );
    // 考虑 buffer 每个字符 [7:0]，最长 w 30 ffff 考虑给 9 个
    reg [3:0] input_cnt;        // 计数，当前在写 buffer 什么位置
    initial input_cnt = 4'h0;
    reg [7:0] buffer[8:0];
    reg read_finished;
    initial read_finished = 1'b0;
    always @(posedge clk) begin
        if (rx_vld) begin
            if (rx_data == 8'h0a) begin
                read_finished <= 1'b1;  // 产生 1 tick 的脉冲信号
                input_cnt     <= 4'h0;
            end
            else begin
                read_finished <= 1'b0;
                input_cnt     <= input_cnt + 1;
                case (input_cnt)
                    4'h0: buffer[0]    <= rx_data;
                    4'h1: buffer[1]    <= rx_data;
                    4'h2: buffer[2]    <= rx_data;
                    4'h3: buffer[3]    <= rx_data;
                    4'h4: buffer[4]    <= rx_data;
                    4'h5: buffer[5]    <= rx_data;
                    4'h6: buffer[6]    <= rx_data;
                    4'h7: buffer[7]    <= rx_data;
                    4'h8: buffer[8]    <= rx_data;
                    default: buffer[8] <= rx_data;
                endcase
            end
        end
        else
            read_finished <= 1'b0;
    end
    // buffer[8:0] 中 [3:2] 固定为地址，[8:5] 可能是要写入的值
    wire [7:0] read_address;       // 直接翻译成读的地址
    wire [15:0] write_value;        // 直接翻译成要写的值
    bytestohex r1(.bytes(buffer[2]),.address(read_address[7:4]));
    bytestohex r2(.bytes(buffer[3]),.address(read_address[3:0]));
    bytestohex w1(.bytes(buffer[5]),.address(write_value[15:12]));
    bytestohex w2(.bytes(buffer[6]),.address(write_value[11:8]));
    bytestohex w3(.bytes(buffer[7]),.address(write_value[7:4]));
    bytestohex w4(.bytes(buffer[8]),.address(write_value[3:0]));

    reg [3:0] mem_out_reg[3:0];

    // 状态机转换
    always @(posedge clk) begin
        curr_state <= next_state;
    end
    always@(*) begin
        case (curr_state)
            S_INPUT: begin
                if (read_finished)
                    next_state = S_PARSE;
                else
                    next_state = S_INPUT;
            end
            S_PARSE: begin
                if (buffer[0] == 8'h65)  // e
                    next_state = S_FETCH;
                else
                    next_state = S_INPUT;
            end
            S_FETCH: begin
                next_state = S_OP;
            end
            S_OP: begin
                if (ir == 16'h0 || ir[15:12] == 4'hf)
                    next_state = S_INPUT;
                else if (ir[15:12] == LC3_LD || ir[15:12] == LC3_LDR || ir[15:12] == LC3_NOT 
                        || ir[15:12] == LC3_ADD || ir[15:12] == LC3_AND)
                    next_state = S_COND;
                else
                    next_state = S_FETCH;
            end
            S_COND: begin
                next_state = S_FETCH;
            end
            default: next_state = S_INPUT;
        endcase
    end

    // Moore 型 “输出部分”（每个状态执行操作）
    always @(posedge clk) begin
        case (curr_state)
            S_PARSE: begin
                if (buffer[0] == 8'h72) begin       // r
                    mem_out_reg[0] <= mem[read_address][15:12];
                    mem_out_reg[1] <= mem[read_address][11:8];
                    mem_out_reg[2] <= mem[read_address][7:4];
                    mem_out_reg[3] <= mem[read_address][3:0];
                end
                else if (buffer[0] == 8'h77) begin  // w
                    mem[read_address] <= write_value;
                end
                else if (buffer[0] == 8'h65) begin  // e
                    pc <= read_address;
                end
            end
            S_FETCH: begin
                ir <= mem[pc];
                pc <= pc + 1;
            end
            S_COND: begin
                if (R[ir[11:9]] == 16'b0)
                    {n, z, p} <= {1'b0, 1'b1, 1'b0};  
                else if (R[ir[11:9]][15] == 1'b0)
                    {n, z, p} <= {1'b0, 1'b0, 1'b1};
                else
                    {n, z, p} <= {1'b1, 1'b0, 1'b0};
            end
            S_OP: begin
                case (ir[15:12])
                    LC3_ADD: begin  // add
                        if (ir[5])
                            R[ir[11:9]] <= (R[ir[8:6]] + {{11{ir[4]}}, ir[4:0]});
                        else
                            R[ir[11:9]] <= R[ir[8:6]] + R[ir[2:0]];
                    end
                    LC3_AND: begin  // and
                        if (ir[5])
                            R[ir[11:9]] <= (R[ir[8:6]] & {{11{ir[4]}}, ir[4:0]});
                        else
                            R[ir[11:9]] <= R[ir[8:6]] & R[ir[2:0]];
                    end
                    LC3_NOT: R[ir[11:9]]                                  <= ~R[ir[8:6]];
                    LC3_BR:  if ({n & ir[11], z & ir[10], p & ir[9]}) pc  <= pc + ir[7:0];
                    LC3_JMP: pc                                           <= R[ir[8:6]][7:0];
                    LC3_LD:  R[ir[11:9]]                                  <= mem[pc + ir[7:0]];
                    LC3_LDR: R[ir[11:9]]                                  <= mem[R[ir[8:6]][7:0] + {{2{ir[5]}}, ir[5:0]}];
                    LC3_LEA: R[ir[11:9]]                                  <= {8'b0, pc + ir[7:0]};
                    LC3_STR: mem[R[ir[8:6]][7:0] + {{2{ir[5]}}, ir[5:0]}] <= R[ir[11:9]];
                    LC3_ST:  mem[pc + ir[7:0]]                            <= R[ir[11:9]];
                endcase
            end
        endcase
    end

    // 输出 LED，当前状态， nzp， 2 位 pc， 四位回显数字
    reg [4:0] time_count;
    always @(posedge clk) begin
        time_count <= time_count + 1;
    end
    
    always@(posedge clk) begin
        hexplay_an <= time_count[4:2];
        case (time_count[4:2])
            3'b000 : hexplay_data <= mem_out_reg[3];
            3'b001 : hexplay_data <= mem_out_reg[2];
            3'b010 : hexplay_data <= mem_out_reg[1];
            3'b011 : hexplay_data <= mem_out_reg[0];
            3'b100 : hexplay_data <= pc[3:0];
            3'b101 : hexplay_data <= pc[7:4];
            3'b110 : hexplay_data <= {1'b0, n, z, p};
            3'b111 : hexplay_data <= {1'b0, curr_state};
            default: hexplay_data <= 4'b0000;
        endcase
    end
endmodule