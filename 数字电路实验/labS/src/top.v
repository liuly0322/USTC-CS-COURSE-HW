`include "tx.v"
`include "rx.v"
`include "mem.v"
`include "bytestohex.v"
`include "hextobytes.v"

module top(input clk,
           rst,
           input rx,
           output tx,
           output reg [2:0] hexplay_an,
           output reg [3:0] hexplay_data);
    
    // 状态机状态
    parameter S_INPUT       = 4'b0000;		// 等待数据写入，可作为初始状态
    parameter S_PARSE       = 4'b0001;		// 解析缓冲区指令
    parameter S_SEND_UPDATE = 4'b0010;
    parameter S_SEND        = 4'b0011;		// 发送内存数据
    parameter S_WRITE       = 4'b0100;      // 写入内存数据
    parameter S_FETCH       = 4'b0101;      // 取指令
    parameter S_IR          = 4'b0110;
    parameter S_OP          = 4'b0111;      // 执行操作
    parameter S_PC          = 4'b1000;      // pc 改为增加后结果
    parameter S_LD          = 4'b1001;      // LD 指令临时跳转状态
    parameter S_LDR         = 4'b1010;      // LDR 指令临时跳转状态
    parameter S_WRITE_MEM   = 4'b1011;      // 执行需要写入的中转状态
    
    reg [3:0] curr_state;
    reg [3:0] next_state;

    initial curr_state = 4'b0000;
    
    // 内存部分
    reg [11:0] address;
    reg [15:0] write;
    reg write_enable;
    initial write_enable = 1'b0;
    wire [15:0] mem_out;
    dist_mem_gen_0 mem(.a(address),.d(write),.clk(clk),.we(write_enable),.spo(mem_out));
    
    // 寄存器
    reg signed [15:0] R[7:0];    // 用于判断 nzp
    reg signed [15:0] temp;      // 加
    reg [2:0] reg_temp;          // 临时保存寄存器信息
    reg [2:0] send_temp;
    reg [11:0] pc;
    reg [11:0] pc_temp;
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
    // 考虑 buffer 每个字符 [7:0]，最长 w 300 ffff 考虑给 10 个
    reg [3:0] input_cnt;        // 计数，当前在写 buffer 什么位置
    initial input_cnt = 4'h0;
    reg [7:0] buffer[9:0];
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
    wire tx_rd;
    reg [2:0] send_count;
    initial send_count = 3'b0;
    wire [7:0] out_ascii[4:0];       // 四个 16 进制数，一个换行
    hextobytes o1(.hex(mem_out[15:12]),.bytes(out_ascii[0]));
    hextobytes o2(.hex(mem_out[11:8]),.bytes(out_ascii[1]));
    hextobytes o3(.hex(mem_out[7:4]),.bytes(out_ascii[2]));
    hextobytes o4(.hex(mem_out[3:0]),.bytes(out_ascii[3]));
    assign out_ascii[4] = 8'h0a;
    reg                 tx_ready;
    initial             tx_ready = 1'b0;
    reg         [7:0]   tx_data;
    tx                  tx_inst(
    .clk                (clk),
    .rst                (rst),
    .tx                 (tx),
    .tx_ready           (tx_ready),
    .tx_rd              (tx_rd),
    .tx_data            (tx_data)
    );
    
    // 输入输出状态机转换
    always @(posedge clk) begin
        curr_state <= next_state;
    end
    always@(*)
    begin
        case (curr_state)
            S_INPUT: begin
                if (read_finished)
                    next_state = S_PARSE;
                else
                    next_state = S_INPUT;
            end
            S_PARSE: begin
                if (buffer[0] == 8'h72) begin       // r
                    next_state   = S_SEND;
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
                    next_state = S_FETCH;
                    pc         = read_address;
                end
            end
            S_FETCH: begin
                next_state   = S_IR;
                address      = pc;
                write_enable = 1'b0;
                pc_temp      = pc + 12'b1;
            end
            S_IR: begin
                next_state = S_OP;
                ir         = mem_out;
                pc         = pc_temp;
            end
            S_PC: begin
                pc         = temp[11:0];
                next_state = S_FETCH;
            end
            S_LD: begin
                R[reg_temp] = mem_out;
                next_state  = S_FETCH;
                if (R[reg_temp] > 16'h0000) 
                    {n,z,p} = {1'b0,1'b0,1'b1};
                else if (R[reg_temp] == 16'h0000)
                    {n,z,p} = {1'b0, 1'b1,1'b0};
                else {n,z,p} = {1'b1,1'b0,1'b0};
            end
            S_LDR: begin
                R[reg_temp] = mem_out;
                next_state  = S_FETCH;
                if (R[reg_temp] > 16'h0000) 
                    {n,z,p} = {1'b0,1'b0,1'b1};
                else if (R[reg_temp] == 16'h0000)
                    {n,z,p} = {1'b0, 1'b1,1'b0};
                else {n,z,p} = {1'b1,1'b0,1'b0};
            end
            S_OP: begin
                case (ir[15:12])
                    4'b0001: begin  // add
                        if (ir[5])
                            R[ir[11:9]] = (R[ir[8:6]] + {{11{ir[4]}}, ir[4:0]});
                        else
                            R[ir[11:9]] = R[ir[8:6]] + R[ir[2:0]];
                        
                        // 条件寄存器
                        if (R[ir[11:9]] > 16'h0000)
                            {n,z,p} = {1'b0, 1'b0, 1'b1};
                        else if (R[ir[11:9]] == 16'h0000)
                            {n,z,p} = {1'b0, 1'b1, 1'b0};
                        else
                            {n,z,p} = {1'b1, 1'b0, 1'b0};
                        
                        next_state = S_FETCH;
                    end
                    4'b0101: begin  // and
                        if (ir[5])
                            R[ir[11:9]] = (R[ir[8:6]] & {{11{ir[4]}}, ir[4:0]});
                        else
                            R[ir[11:9]] = R[ir[8:6]] & R[ir[2:0]];
                        
                        // 条件寄存器
                        if (R[ir[11:9]] > 16'h0000)
                            {n,z,p} = {1'b0, 1'b0, 1'b1};
                        else if (R[ir[11:9]] == 16'h0000)
                            {n,z,p} = {1'b0, 1'b1, 1'b0};
                        else
                            {n,z,p} = {1'b1, 1'b0, 1'b0};
                        
                        next_state = S_FETCH;
                    end
                    4'b1001: begin  // not
                        temp    = (~R[ir[8:6]]);
                        R[ir[11:9]] = temp;
                        
                        //设置条件寄存器
                        if (R[ir[11:9]] > 16'h0000)
                            {n,z,p} = {1'b0, 1'b0, 1'b1};
                        else if (R[ir[11:9]] == 16'h0000)
                            {n,z,p} = {1'b0, 1'b1, 1'b0};
                        else
                            {n,z,p} = {1'b1, 1'b0, 1'b0};
                        
                        next_state = S_FETCH;
                    end
                    4'b0000: begin  // br
                        if (ir == 16'h0)
                            next_state = S_INPUT;
                        else if ({n & ir[11], z & ir[10], p & ir[9]}) begin
                            temp[11:0]   = pc + {{3{ir[8]}}, ir[8:0]};
                            next_state   = S_PC;
                        end
                        else next_state = S_FETCH;
                    end
                    4'b1100: begin  // jmp
                        pc = R[ir[8:6]][11:0];
                        next_state = S_FETCH;
                    end
                    4'b0010: begin  // ld
                        address      = pc + {{3{ir[8]}}, ir[8:0]};
                        write_enable = 1'b0;
                        reg_temp = ir[11:9];
                        next_state = S_LD;
                    end
                    4'b0110: begin  // ldr
                        address      = R[ir[8:6]] + {{6{ir[5]}}, ir[5:0]};
                        write_enable = 1'b0;
                        reg_temp = ir[11:9];
                        next_state = S_LDR;
                    end
                    4'b1110: begin  // lea
                        R[ir[11:9]] = {4'b0, pc + {{3{ir[8]}}, ir[8:0]}};
                        next_state = S_FETCH;
                    end
                    4'b0111: begin  // str
                        address      = R[ir[8:6]] + {{6{ir[5]}}, ir[5:0]};
                        write_enable = 1'b1;
                        write        = R[ir[11:9]];
                        next_state   = S_WRITE_MEM;
                    end
                    4'b0011: begin  // st
                        address      = pc + {{3{ir[8]}}, ir[8:0]};
                        write_enable = 1'b1;
                        write        = R[ir[11:9]];    
                        next_state   = S_WRITE_MEM; 
                    end
                    default: next_state = S_INPUT;
                endcase
            end
            S_WRITE: begin
                write_enable = 1'b0;
                next_state   = S_INPUT;
            end
            S_WRITE_MEM: begin
                write_enable = 1'b0;
                next_state   = S_FETCH;
            end
            S_SEND: begin // 读取 out_ascii 并发送
                if (tx_rd) begin
                    send_temp  = send_count + 3'b1;
                    next_state = S_SEND_UPDATE;
                end
                else begin
                    next_state = S_SEND;
                    tx_data    = out_ascii[send_count];
                    tx_ready   = 1'b1;
                end
            end
            S_SEND_UPDATE: begin
                if (send_temp == 3'b101 || send_temp > 3'b101) begin
                    send_count = 3'b0;
                    tx_ready   = 1'b0;
                    next_state = S_INPUT;
                end
                else begin
                    send_count = send_temp;
                    next_state = S_SEND;
                end
            end
            default: next_state = S_INPUT;
        endcase
    end

    // 输出 LED ( debug 用)
    reg [4:0] time_count;
    always @(posedge clk) begin
        if (time_count == 5'b10111)
            time_count <= 5'b00000;
        else
            time_count <= time_count + 1;
    end
    
    always@(posedge clk) begin
        hexplay_an <= time_count[4:2];
        case (time_count[4:2])
            3'b000 : hexplay_data <= curr_state[0];
            3'b001 : hexplay_data <= curr_state[1];
            3'b010 : hexplay_data <= curr_state[2];
            3'b011 : hexplay_data <= curr_state[3];
            3'b100 : hexplay_data <= tx_ready;
            3'b101 : hexplay_data <= tx_rd;
            default: hexplay_data <= 4'b0000;
        endcase
    end
endmodule
