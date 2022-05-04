module  pdu(
        input clk,            // clk100mhz
        input rstn,           // cpu_resetn

        input step,           // btnu
        input cont,           // btnd
        input chk,            // btnr
        input data,           // btnc
        input del,            // btnl
        input [15:0] x,       // sw15-0

        output stop,          // led16r
        output [15:0] led,    // led15-0
        output [7:0] an,      // an7-0
        output [6:0] seg,     // ca-cg
        output [2:0] seg_sel, // led17

        output clk_cpu,       // cpu's clk
        output rst_cpu,       // cpu's rst

        // IO_BUS
        input [7:0] io_addr,
        input [31:0] io_dout,
        input io_we,
        input io_rd,
        output [31:0] io_din,

        // Debug_BUS
        input [31:0] pc,
        output [15:0] chk_addr,
        input [31:0] chk_data
    );

reg [15:0] rstn_r;
wire rst;               // 复位信号，高电平有效
wire clk_pdu;           // PDU工作时钟
wire clk_db;            // 去抖动计数器时钟
reg stop_r, stop_n;

reg [19:0] cnt_clk_r;   // 时钟分频、数码管刷新计数器
reg [4:0] cnt_sw_db_r;
reg [15:0] x_db_r, x_db_1r;
reg xx_r, xx_1r;
wire x_p;
reg [3:0] x_hd_t;

wire [4:0] btn;
reg [4:0] cnt_btn_db_r;
reg [4:0] btn_db_r, btn_db_1r;
wire step_p, cont_p, chk_p, data_p, del_p;

reg [15:0] led_data_r;  // 指示灯led15-0数据
reg [31:0] seg_data_r;  // 数码管输出数据
reg seg_rdy_r;          // 数码管准备好标志
reg [31:0] swx_data_r;  // 开关输入数据
reg swx_vld_r;          // 开关输入有效标志
reg [31:0] cnt_data_r;  // 性能计数器数据

reg [31:0] tmp_r;       // 临时编辑数据
reg [31:0] brk_addr_r;  // 断点地址
reg [15:0] chk_addr_r;  // 查看地址
reg [31:0] io_din_t;

reg led_sel_r;
reg [2:0] seg_sel_r;
reg [31:0] disp_data_t;
reg [7:0] an_t;
reg [3:0] hd_t;
reg [6:0] seg_t;

assign rst = rstn_r[15];    // 经处理后的复位信号，高电平有效
assign rst_cpu = rst;
assign clk_pdu = cnt_clk_r[1];      // PDU工作时钟25MHz
assign clk_db = cnt_clk_r[16];      // 去抖动计数器时钟763Hz（周期约1.3ms）
assign clk_cpu = clk_pdu & stop_n;  // CPU工作时钟

assign stop = stop_r;
assign led = (led_sel_r)? chk_addr : led_data_r;
assign an = an_t;
assign seg = seg_t;
assign seg_sel = seg_sel_r;

assign io_din = io_din_t;
assign chk_addr = chk_addr_r;

assign btn ={step, cont, chk, data, del};
assign x_p = xx_r ^ xx_1r;
assign step_p = btn_db_r[4] & ~ btn_db_1r[4];
assign cont_p = btn_db_r[3] & ~ btn_db_1r[3];
assign chk_p = btn_db_r[2] & ~ btn_db_1r[2];
assign data_p = btn_db_r[1] & ~ btn_db_1r[1];
assign del_p = btn_db_r[0] & ~ btn_db_1r[0];


///////////////////////////////////////////////
// 复位处理：异步复位、同步和延迟释放
///////////////////////////////////////////////
always @(posedge clk, negedge rstn) begin
    if (!rstn)
        rstn_r <= 16'hFFFF;
    else
        rstn_r <= {rstn_r[14:0], 1'b0};
end


///////////////////////////////////////////////
// 时钟分频
///////////////////////////////////////////////
always @(posedge clk or posedge rst) begin // changed
    if (rst)
        cnt_clk_r <= 20'h0;
    else
        cnt_clk_r <= cnt_clk_r + 20'h1;
end


///////////////////////////////////////////////
// 开关sw去抖动
///////////////////////////////////////////////
always @(posedge clk_db or posedge rst) begin  // changed
    if (rst)
        cnt_sw_db_r <= 5'h0;
    else if ((|(x ^ x_db_r)) & (~cnt_sw_db_r[4]))   // changed
        cnt_sw_db_r <= cnt_sw_db_r + 5'h1;
    else
        cnt_sw_db_r <= 5'h0;
end

always@(posedge clk_db or posedge rst) begin // changed
    if (rst) begin
        x_db_r <= x;
        x_db_1r <= x;
        xx_r <= 1'b0;
    end
    else if (cnt_sw_db_r[4]) begin    // 信号稳定约21ms后输出
        x_db_r <= x;
        x_db_1r <= x_db_r;
        xx_r <= ~xx_r;
    end
end

always @(posedge clk_pdu or posedge rst) begin // changed
    if (rst)
        xx_1r <= 1'b0;
    else
        xx_1r <= xx_r;
end


///////////////////////////////////////////////
// 按钮btn去抖动
///////////////////////////////////////////////
always @(posedge clk_db or posedge rst) begin  // changed
    if (rst)
        cnt_btn_db_r <= 5'h0;
    else if ((|(btn ^ btn_db_r)) & (~cnt_btn_db_r[4]))  // changed
        cnt_btn_db_r <= cnt_btn_db_r + 5'h1;
    else
        cnt_btn_db_r <= 5'h0;
end

always@(posedge clk_db or posedge rst) begin    // changed
    if (rst)
        btn_db_r <= btn;
    else if (cnt_btn_db_r[4])
        btn_db_r <= btn;
end

always @(posedge clk_pdu or posedge rst) begin     // changed
    if (rst)
        btn_db_1r <= btn;
    else
        btn_db_1r <= btn_db_r;
end


///////////////////////////////////////////////
// 控制CPU运行方式
///////////////////////////////////////////////
reg [1:0] cs, ns;
parameter STOP = 2'b00, STEP = 2'b01, RUN = 2'b10;

always @(posedge clk_pdu or posedge rst) begin  // changed
    if (rst)
        cs <= STOP;
    else
        cs <= ns;
end

always @* begin
    ns = cs;
    case (cs)
        STOP: begin
            if (step_p)
                ns = STEP;
            else if (cont_p)
                ns = RUN;
        end
        STEP:
            ns = STOP;
        RUN: begin
            if (brk_addr_r == pc)
                ns = STOP;
        end
        default:
            ns = STOP;
    endcase
end

always @(posedge clk_pdu or posedge rst) begin  // changed
    if (rst)
        stop_r <= 1'b1;
    else if (ns == STOP)
        stop_r <= 1'b1;
    else
        stop_r <= 1'b0;
end
// assign stop_n = ~stop_r ?
always @(negedge clk_pdu or posedge rst) begin  // changed
    if (rst)
        stop_n <= 1'b0; // changed
    else
        stop_n <= ~stop_r; // changed
end


///////////////////////////////////////////////
// CPU输入/输出
///////////////////////////////////////////////
always @(posedge clk_pdu or posedge rst) begin    // CPU输出 changed
    if (rst) begin
        led_data_r <= 16'hFFFF;
        seg_data_r <= 32'h12345678;
    end
    else if (io_we) begin
        case (io_addr)
            8'h00:
                led_data_r <= io_dout[15:0];
            8'h0C:
                seg_data_r <= io_dout;
            default:
                ;
        endcase
    end
end

always @(posedge clk_pdu or posedge rst) begin  // changed
    if (rst)
        seg_rdy_r <= 1;
    else if (io_we & (io_addr == 8'h0C))
        seg_rdy_r <= 0;
    else if (x_p | del_p)
        seg_rdy_r <= 1;
end

always @(*) begin    // CPU输入
    case (io_addr)
        8'h04:
            io_din_t = {{11{1'b0}}, step, cont, chk, data, del, x};
        8'h08:
            io_din_t = {{31{1'b0}}, seg_rdy_r};
        8'h10:
            io_din_t = {{31{1'b0}}, swx_vld_r};
        8'h14:
            io_din_t = swx_data_r;
        8'h18:
            io_din_t = cnt_data_r;
        default:
            io_din_t = 32'h0;
    endcase
end

always @(posedge clk_pdu or posedge rst) begin  // changed
    if (rst)
        swx_vld_r <= 0;
    else if (data_p & ~swx_vld_r)
        swx_vld_r <= 1;
    else if (io_rd & (io_addr == 8'h14))
        swx_vld_r <= 0;
end


///////////////////////////////////////////////
// 性能计数器
///////////////////////////////////////////////
always@(posedge clk_cpu or posedge rst) begin // changed
    if(rst)
        cnt_data_r <= 32'h0;
    else
        cnt_data_r <= cnt_data_r + 32'h1;
end

///////////////////////////////////////////////
// 开关编辑数据
///////////////////////////////////////////////
always @* begin    // 开关输入编码
    case (x_db_r ^ x_db_1r )
        16'h0001:
            x_hd_t = 4'h0;
        16'h0002:
            x_hd_t = 4'h1;
        16'h0004:
            x_hd_t = 4'h2;
        16'h0008:
            x_hd_t = 4'h3;
        16'h0010:
            x_hd_t = 4'h4;
        16'h0020:
            x_hd_t = 4'h5;
        16'h0040:
            x_hd_t = 4'h6;
        16'h0080:
            x_hd_t = 4'h7;
        16'h0100:
            x_hd_t = 4'h8;
        16'h0200:
            x_hd_t = 4'h9;
        16'h0400:
            x_hd_t = 4'hA;
        16'h0800:
            x_hd_t = 4'hB;
        16'h1000:
            x_hd_t = 4'hC;
        16'h2000:
            x_hd_t = 4'hD;
        16'h4000:
            x_hd_t = 4'hE;
        16'h8000:
            x_hd_t = 4'hF;
        default:
            x_hd_t = 4'h0;
    endcase
end

always @(posedge clk_pdu or posedge rst) begin  // changed
    if (rst)
        tmp_r <= 32'h0;
    else if (x_p)
        tmp_r <= {tmp_r[27:0], x_hd_t};      // x_hd_t + tmp_r << 4
    else if (del_p)
        tmp_r <= {{4{1'b0}}, tmp_r[31:4]}; // tmp_r >> 4
    else if ((cont_p & stop) | (data_p & ~swx_vld_r))
        tmp_r <= 32'h0;
    else if (chk_p & stop)
        tmp_r <= tmp_r + 32'h1;
end

always @(posedge clk_pdu or posedge rst) begin  // changed
    if (rst) begin
        chk_addr_r <= 16'h0;
        brk_addr_r <= 32'h0;
    end
    else if (data_p & ~swx_vld_r)
        swx_data_r <= tmp_r;
    else if (cont_p & stop)
        brk_addr_r <= tmp_r;
    else if (chk_p & stop)
        chk_addr_r <= tmp_r[15:0];
end


///////////////////////////////////////////////
// led15-0指示灯显示
///////////////////////////////////////////////
always @(posedge clk_pdu or posedge rst) begin  // changed
    if (rst)
        led_sel_r <= 1'b0;
    else if (io_we && (io_addr == 8'h00))
        led_sel_r <= 1'b0;
    else if (chk_p)
        led_sel_r <= 1'b1;
end


///////////////////////////////////////////////
// 数码管多用途显示
///////////////////////////////////////////////
always @(posedge clk_pdu or posedge rst) begin    // 数码管显示数据选择 changed
    if (rst)
        seg_sel_r <= 3'b001;
    else if (io_we & (io_addr == 8'h0C))
        seg_sel_r <= 3'b001;   // 输出
    else if (x_p | del_p)
        seg_sel_r <= 3'b010;                  // 编辑
    else if (chk_p & stop)
        seg_sel_r <= 3'b100;                 // 调试
end

always @* begin
    case (seg_sel_r)
        3'b001:
            disp_data_t = seg_data_r;
        3'b010:
            disp_data_t = tmp_r;
        3'b100:
            disp_data_t = chk_data;
        default:
            disp_data_t = tmp_r;
    endcase
end

always @(*) begin          // 数码管扫描
    case (cnt_clk_r[19:17])  // 刷新频率约为95Hz
        3'b000: begin
            an_t = 8'b1111_1110;
            hd_t = disp_data_t[3:0];
        end
        3'b001: begin
            an_t = 8'b1111_1101;
            hd_t = disp_data_t[7:4];
        end
        3'b010: begin
            an_t = 8'b1111_1011;
            hd_t = disp_data_t[11:8];
        end
        3'b011: begin
            an_t = 8'b1111_0111;
            hd_t = disp_data_t[15:12];
        end
        3'b100: begin
            an_t = 8'b1110_1111;
            hd_t = disp_data_t[19:16];
        end
        3'b101: begin
            an_t = 8'b1101_1111;
            hd_t = disp_data_t[23:20];
        end
        3'b110: begin
            an_t = 8'b1011_1111;
            hd_t = disp_data_t[27:24];
        end
        3'b111: begin
            an_t = 8'b0111_1111;
            hd_t = disp_data_t[31:28];
        end
        default:
            ;
    endcase
end

always @ (*) begin    //7段译码
    case(hd_t)
        4'b1111:
            seg_t = 7'b0111000;
        4'b1110:
            seg_t = 7'b0110000;
        4'b1101:
            seg_t = 7'b1000010;
        4'b1100:
            seg_t = 7'b0110001;
        4'b1011:
            seg_t = 7'b1100000;
        4'b1010:
            seg_t = 7'b0001000;
        4'b1001:
            seg_t = 7'b0001100;
        4'b1000:
            seg_t = 7'b0000000;
        4'b0111:
            seg_t = 7'b0001111;
        4'b0110:
            seg_t = 7'b0100000;
        4'b0101:
            seg_t = 7'b0100100;
        4'b0100:
            seg_t = 7'b1001100;
        4'b0011:
            seg_t = 7'b0000110;
        4'b0010:
            seg_t = 7'b0010010;
        4'b0001:
            seg_t = 7'b1001111;
        4'b0000:
            seg_t = 7'b0000001;
        default:
            seg_t = 7'b1111111;
    endcase
end

endmodule
