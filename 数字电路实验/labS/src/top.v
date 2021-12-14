`include "tx.v"
`include "rx.v"
`include "mem.v"
`include "bytestohex.v"

module top(input clk,
           rst,
           input rx,
           output tx);
    
    // 状态机状态
    parameter S_INPUT = 4'b0000;		// 等待数据写入，可作为初始状态
    parameter S_PARSE = 4'b0001;		// 解析缓冲区指令
    parameter S_READ = 4'b0010;		    // 从内存中读取数据
    parameter S_SEND = 4'b0011;		    // 发送内存数据
    parameter S_WRITE = 4'b0100;        // 写入内存数据
    parameter S_EXEC  = 4'b0101;        // 执行命令
    
    reg [3:0] curr_state;
    reg [3:0] next_state;
    
    // 内存部分
    reg [11:0] address;
    reg [15:0] write;
    reg write_enable;
    wire [15:0] mem_out;
    dist_mem_gen_0 mem(.a(address),.d(write),.clk(clk),.we(write_enable),.spo(mem_out));
    
    // 读端口 & buffer 区
    wire        [7:0]   rx_data;
    rx                  rx_inst(
    .clk                (clk),
    .rst                (rst),
    .rx                 (rx),
    .rx_vld             (rx_vld),
    .rx_data            (rx_data)
    );
    // 考虑 buffer 每个字符 [7:0]，最长 w 300 ffff 考虑给 10 个
    reg [3:0] input_cnt;        // 计数，当前在写 buffer 什么位置
    reg [7:0] buffer[9:0];
    reg read_finished;
    always @(posedge clk) begin
        if (rx_vld) begin
            if (rx_data == 8'h0a) begin
                read_finished <= 1'b1;  // 产生 1 tick 的脉冲信号
                input_cnt     <= 4'h0;
            end
            else begin
                input_cnt <= input_cnt + 1;
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
                    4'h9: buffer[9]    <= rx_data;
                    default: buffer[9] <= rx_data;
                endcase
            end
        end
        else
            read_finished <= 1'b0;
    end
    // buffer[9:0] 中 [4:2] 固定为地址，[9:6] 可能是要写入的值
    wire [11:0] read_address;       // 直接翻译成读的地址
    wire [15:0] write_value;        // 直接翻译成要写的值
    bytestohex r1(.bytes(buffer[2]),.address(read_address[11:8]));
    bytestohex r2(.bytes(buffer[3]),.address(read_address[7:4]));
    bytestohex r3(.bytes(buffer[4]),.address(read_address[3:0]));
    bytestohex w1(.bytes(buffer[6]),.address(write_value[15:12]));
    bytestohex w2(.bytes(buffer[7]),.address(write_value[11:8]));
    bytestohex w3(.bytes(buffer[8]),.address(write_value[7:4]));
    bytestohex w4(.bytes(buffer[9]),.address(write_value[3:0]));
    
    // 写端口
    wire                tx_ready;
    wire        [7:0]   tx_data;
    assign  tx_ready = rx_vld;
    tx                  tx_inst(
    .clk                (clk),
    .rst                (rst),
    .tx                 (tx),
    .tx_ready           (tx_ready),
    .tx_rd              (tx_rd),
    .tx_data            (tx_data)
    );
    
    // 状态机转换
    always @(posedge clk) begin
        next_state <= curr_state;
    end
    always@(*)
    begin
        case (curr_state)
            S_INPUT: next_state = read_finished ? S_PARSE : S_INPUT;
            S_PARSE: begin
                if (buffer[0] == 8'h72) begin       // r
                    next_state   = S_READ;
                    address      = read_address;
                    write_enable = 1'b0;
                end
                else if (buffer[0] == 8'h77) begin  // w
                    next_state   = S_WRITE;
                    address      = read_address;
                    write        = write_value;
                    write_enable = 1'b1;
                end
                else begin                          // e
                    next_state = S_EXEC;
                    // to be done
                end
            end
            S_READ: next_state  = S_SEND;
            default: next_state = S_INPUT;
        endcase
    end
    
endmodule
